// Command generator writes the search methods of the binsearch package.
//
// Every method is the same handful of lines around one cgo call, repeated for
// each combination of layout, search, byte order and type. Writing them by hand
// means that a change to the argument checks or to the cgo call has to be made
// in 128 places. Run it with "make generate".
package main

import (
	"bytes"
	"fmt"
	"go/format"
	"os"
	"strings"
)

// width describes one of the unsigned integer types the searches support.
type width struct {
	Bits  int    // Number of bits.
	Go    string // Name of the Go type.
	C     string // Name of the C type.
	Suff  string // Suffix of the Go method name.
	Bytes int    // Size in bytes.
}

var widths = []width{
	{8, "uint8", "uint8_t", "Uint8", 1},
	{16, "uint16", "uint16_t", "Uint16", 2},
	{32, "uint32", "uint32_t", "Uint32", 4},
	{64, "uint64", "uint64_t", "Uint64", 8},
}

// order describes one of the two byte orders.
type order struct {
	Go   string // Suffix of the Go method name.
	C    string // Infix of the C function name.
	Text string // Name used in the documentation.
}

var orders = []order{
	{"BE", "be", "big-endian"},
	{"LE", "le", "little-endian"},
}

// kind describes one of the operations, in one of the two layouts.
type kind struct {
	Method string // Verb of the Go method name: FindFirst, FindLast, HasNext, HasPrev.
	CName  string // Verb of the C function name: find_first, find_last, has_next, has_prev.
	Col    bool   // Column mode rather than row mode.
	Sub    bool   // Matches a range of bits rather than the whole value.
}

// layoutText returns the wording that describes the layout a kind reads.
func (k kind) layoutText(o order, w width) string {
	if k.Col {
		return fmt.Sprintf("a contiguous array of %s %d bit unsigned integers", o.Text, w.Bits)
	}

	return fmt.Sprintf("a memory mapped file of adjacent blocks of sorted binary data holding %s %d bit unsigned integers", o.Text, w.Bits)
}

// goName returns the name of the generated method.
func (k kind) goName(o order, w width) string {
	name := k.Method
	if k.Sub {
		name += "Sub"
	}

	if k.Col {
		name = "Col" + name
	}

	return name + o.Go + w.Suff
}

// cName returns the name of the C function the method calls.
func (k kind) cName(o order, w width) string {
	name := k.CName
	if k.Sub {
		name += "_sub"
	}

	if k.Col {
		name = "col_" + name
	}

	return fmt.Sprintf("%s_%s_%s", name, o.C, w.C)
}

// find reports whether the kind returns item numbers rather than a boolean.
func (k kind) find() bool {
	return strings.HasPrefix(k.Method, "Find")
}

// many reports whether the kind searches a slice of values in one call.
func (k kind) many() bool {
	return k.Method == "FindMany"
}

var kinds = []kind{
	{"FindFirst", "find_first", false, false},
	{"FindLast", "find_last", false, false},
	{"FindFirst", "find_first", false, true},
	{"FindLast", "find_last", false, true},
	{"FindRange", "find_range", false, false},
	{"FindRange", "find_range", false, true},
	{"FindMany", "find_many", false, false},
	{"FindMany", "find_many", false, true},
	{"HasNext", "has_next", false, false},
	{"HasNext", "has_next", false, true},
	{"HasPrev", "has_prev", false, false},
	{"HasPrev", "has_prev", false, true},
	{"FindFirst", "find_first", true, false},
	{"FindLast", "find_last", true, false},
	{"FindFirst", "find_first", true, true},
	{"FindLast", "find_last", true, true},
	{"FindRange", "find_range", true, false},
	{"FindRange", "find_range", true, true},
	{"FindMany", "find_many", true, false},
	{"FindMany", "find_many", true, true},
	{"HasNext", "has_next", true, false},
	{"HasNext", "has_next", true, true},
	{"HasPrev", "has_prev", true, false},
	{"HasPrev", "has_prev", true, true},
}

// params returns the Go parameter list of the method.
func (k kind) params(w width) string {
	var head string

	switch {
	case k.Col && k.Sub:
		head = "offset uint64, bitstart, bitend uint8"
	case k.Col:
		head = "offset"
	case k.Sub:
		head = "offset, blklen, blkpos uint64, bitstart, bitend uint8"
	default:
		head = "offset, blklen, blkpos"
	}

	var tail string

	switch k.Method {
	case "HasNext":
		tail = "pos, last uint64"
	case "HasPrev":
		tail = "first, pos uint64"
	default:
		tail = "first, last uint64"
	}

	if k.many() {
		return fmt.Sprintf("%s, %s, search []%s", head, tail, w.Go)
	}

	return fmt.Sprintf("%s, %s, search %s", head, tail, w.Go)
}

// results returns the Go result list of the method.
func (k kind) results() string {
	switch {
	case k.many():
		return "([]uint64, error)"
	case k.find():
		return "(uint64, uint64, uint64, error)"
	}

	return "(bool, uint64, error)"
}

// bound returns the name of the argument that bounds the items the search reads.
func (k kind) bound() string {
	if k.Method == "HasPrev" {
		return "pos"
	}

	return "last"
}

// invalid returns the result the method reports when its arguments do not
// describe a range inside the memory map: the one of a value that was not
// found, plus the reason.
func (k kind) invalid() string {
	switch {
	case k.many():
		return "return nil, err"
	case k.Method == "FindRange":
		// The reported range must be empty, not the one that was asked for: a
		// caller that iterates it without looking at the error would otherwise
		// walk every item of the range it passed in.
		return "return 0, last, last, err"
	case k.find():
		return "return last, first, last, err"
	}

	return "return false, pos, err"
}

// cargs returns the argument list of the cgo call.
func (k kind) cargs(w width) string {
	args := []string{}
	if k.Col {
		args = append(args, fmt.Sprintf("(*C.%s)(src)", w.C))
	} else {
		args = append(args, "(*C.uint8_t)(src)", "C.uint64_t(blklen)", "C.uint64_t(blkpos)")
	}

	if k.Sub {
		args = append(args, "C.uint8_t(bitstart)", "C.uint8_t(bitend)")
	}

	switch k.Method {
	case "HasNext":
		args = append(args, "&cpos", "C.uint64_t(last)")
	case "HasPrev":
		args = append(args, "C.uint64_t(first)", "&cpos")
	case "FindMany":
		args = append(args,
			"C.uint64_t(first)", "C.uint64_t(last)",
			fmt.Sprintf("(*C.%s)(&search[0])", w.C),
			"(*C.uint64_t)(&pos[0])", "C.uint64_t(len(search))",
			"C.binsearch_prefetch_t(mf.Prefetch)")

		return strings.Join(args, ", ")
	default:
		args = append(args, "&cfirst", "&clast")
	}

	args = append(args, fmt.Sprintf("C.%s(search)", w.C))

	return strings.Join(args, ", ")
}

// doc returns the documentation comment of the method, wrapped to 100 columns.
func (k kind) doc(o order, w width) string {
	name := k.goName(o, w)
	layout := k.layoutText(o, w)

	var body string

	switch k.Method {
	case "HasNext", "HasPrev":
		neighbour, walk, finder := "after", "forward", "FindFirst"
		if k.Method == "HasPrev" {
			neighbour, walk, finder = "before", "backward", "FindLast"
		}

		if k.Sub {
			finder += "Sub"
		}

		if k.Col {
			finder = "Col" + finder
		}

		finder += o.Go + w.Suff

		subject := "the item " + neighbour + " pos"
		if k.Sub {
			subject = "the bits from bitstart to bitend of the item " + neighbour + " pos"
		}

		body = fmt.Sprintf("%s reports whether %s matches the search value in %s. Set pos to the item returned by %s to walk %s over the matching items. It returns the result and the updated position.",
			name, subject, layout, finder, walk)
	case "FindRange":
		subject := "a value"
		if k.Sub {
			subject = "the bits from bitstart to bitend of a value"
		}

		finder := "FindFirst"
		if k.Sub {
			finder += "Sub"
		}

		if k.Col {
			finder = "Col" + finder
		}

		last := strings.Replace(finder, "FindFirst", "FindLast", 1)

		body = fmt.Sprintf("%s narrows a range to the items equal to %s in %s. The values must be sorted in ascending order. It returns the number of items equal to the searched value, which is 0 when it is absent, and the half-open range holding them: the first item equal to it and the item past the last one. The range is empty when the value is absent, at the position where it would be inserted. It computes in one descent what %s and %s compute in two.",
			name, subject, layout, finder+o.Go+w.Suff, last+o.Go+w.Suff)
	case "FindMany":
		subject := "each of several values"
		if k.Sub {
			subject = "the bits from bitstart to bitend of each of several values"
		}

		finder := "FindFirst"
		if k.Sub {
			finder += "Sub"
		}

		if k.Col {
			finder = "Col" + finder
		}

		body = fmt.Sprintf("%s searches for the first occurrence of %s in %s. The values must be sorted in ascending order. It returns, for each searched value, what %s returns for it: the item number when found or last when not found. The searches are advanced in lockstep so that their cache misses overlap, which is about three to four times as fast per value on data larger than the CPU cache and still held in RAM. On a file too large for RAM it asks for the pages of a batch before reading them, as the Prefetch field of the mapping directs.",
			name, subject, layout, finder+o.Go+w.Suff)
	default:
		occurrence := "first"
		if k.Method == "FindLast" {
			occurrence = "last"
		}

		subject := "a value"
		if k.Sub {
			subject = "the bits from bitstart to bitend of a value"
		}

		body = fmt.Sprintf("%s searches for the %s occurrence of %s in %s. The values must be sorted in ascending order. It returns the item number when found or the initial last value when not found, plus the lower bound of the searched value in first and last, or the upper bound for a FindLast search.",
			name, occurrence, subject, layout)
	}

	body += " " + guardDoc(k)

	return wrap(body, 98)
}

// guardDoc returns the sentence that describes what the method does with
// arguments that fall outside the memory map.
func guardDoc(k kind) string {
	bound := "last"
	if k.Method == "HasPrev" {
		bound = "pos"
	}

	errs := "ErrNotMapped or ErrOutOfRange"
	if k.Col {
		errs = "ErrNotMapped, ErrMisaligned or ErrOutOfRange"
	}

	if k.Sub {
		errs = "ErrBitRange, " + errs
	}

	return fmt.Sprintf("It returns an error wrapping %s, and the results of a value that was not found, when offset and %s do not describe a range of items inside the memory mapped file%s.",
		errs, bound, subDoc(k))
}

// subDoc returns the clause that names the other reason a Sub search refuses to
// run.
func subDoc(k kind) string {
	if k.Sub {
		return ", or when the bit range is not inside the searched type"
	}

	return ""
}

// wrap breaks a line of documentation into comment lines of at most n columns.
func wrap(s string, n int) string {
	var (
		out  strings.Builder
		line = "//"
	)

	for _, word := range strings.Fields(s) {
		if len(line)+1+len(word) > n {
			out.WriteString(line + "\n")

			line = "//"
		}

		line += " " + word
	}

	out.WriteString(line)

	return out.String()
}

func main() {
	// The files are written to the working directory, so a run from the wrong
	// one would leave them outside the package.
	if _, err := os.Stat("binsearch.go"); err != nil {
		fmt.Fprintln(os.Stderr, "generator: run it from the package directory, where binsearch.go is")
		os.Exit(1)
	}

	var b bytes.Buffer

	b.WriteString(`// Code generated by "make generate"; DO NOT EDIT.

package binsearch

/*
`)

	// The cgo directives of the search functions. None of them retains a
	// pointer it is given and none of them calls back into Go, so the arguments
	// can stay on the goroutine stack instead of being moved to the heap.
	names := []string{}

	for _, k := range kinds {
		for _, o := range orders {
			for _, w := range widths {
				names = append(names, k.cName(o, w))
			}
		}
	}

	sorted := append([]string{}, names...)
	for i := range sorted {
		for j := i + 1; j < len(sorted); j++ {
			if sorted[j] < sorted[i] {
				sorted[i], sorted[j] = sorted[j], sorted[i]
			}
		}
	}

	b.WriteString(`// None of the C functions retains a pointer it is given and none of them
// calls back into Go, so the arguments of the wrappers can stay on the
// goroutine stack instead of being moved to the heap: without these
// directives cgo has to assume the opposite and every search call costs one
// allocation for the first, last or pos argument.
`)

	for _, n := range sorted {
		fmt.Fprintf(&b, "#cgo noescape %s\n#cgo nocallback %s\n", n, n)
	}

	b.WriteString("\n#include \"../../c/src/binsearch/binsearch.h\"\n*/\nimport \"C\"\n")

	for _, k := range kinds {
		for _, o := range orders {
			for _, w := range widths {
				writeMethod(&b, k, o, w)
			}
		}
	}

	write("binsearch_search.go", b.Bytes())
	write("binsearch_guard_test.go", guards())
	write("binsearch_range_test.go", ranges())
}

// write formats the generated source and stores it.
func write(name string, content []byte) {
	src, err := format.Source(content)
	if err != nil {
		fmt.Fprintln(os.Stderr, "generator:", name, err)
		os.Exit(1)
	}

	if err := os.WriteFile(name, src, 0o600); err != nil {
		fmt.Fprintln(os.Stderr, "generator:", name, err)
		os.Exit(1)
	}
}

// guards returns a test that calls every search method with arguments that fall
// outside the memory map, and with a bit range that falls outside the searched
// type, and checks that the method reports the value as not found.
func guards() []byte {
	var b bytes.Buffer

	b.WriteString(`// Code generated by "make generate"; DO NOT EDIT.

package binsearch

import (
	"testing"

	"github.com/stretchr/testify/require"
)

// badOffset is past the end of every fixture, so no search can read from it.
const badOffset = uint64(1) << 60

// TestSearchGuards checks that every search method reports the value as not
// found instead of reading outside the memory mapped file.
func TestSearchGuards(t *testing.T) {
`)

	for _, k := range kinds {
		for _, o := range orders {
			for _, w := range widths {
				writeGuard(&b, k, o, w)
			}
		}
	}

	b.WriteString("}\n")

	return b.Bytes()
}

// writeGuard appends the checks of one method to b.
func writeGuard(b *bytes.Buffer, k kind, o order, w width) {
	name := k.goName(o, w)

	// The arguments before the bound, with an offset that is past the end.
	head := "badOffset"
	if !k.Col {
		head += ", 8, 0"
	}

	if k.Sub {
		head += fmt.Sprintf(", 0, %d", (w.Bits - 1))
	}

	args := head + ", 0, 1, 0"
	if k.many() {
		// The batch is not empty, so the guard is reached rather than the
		// shortcut that returns an empty result.
		args = head + fmt.Sprintf(", 0, 1, []%s{0}", w.Go)
	}

	fmt.Fprintf(b, "\n\tt.Run(%q, func(t *testing.T) {\n", name)

	writeGuardCase(b, k, name, args, "ErrOutOfRange", ":=")

	if k.Sub {
		// A bit range that reaches past the end of the searched type would make
		// the C code shift by more than the width of the value.
		bad := head[:len(head)-len(fmt.Sprintf(", 0, %d", (w.Bits-1)))] + fmt.Sprintf(", 0, %d", w.Bits)
		tail := ", 0, 1, 0"

		if k.many() {
			tail = fmt.Sprintf(", 0, 1, []%s{0}", w.Go)
		}

		b.WriteString("\n")
		writeGuardCase(b, k, name, bad+tail, "ErrBitRange", "=")
	}

	b.WriteString("\t})\n")
}

// writeGuardCase appends one rejected call and the results it must report.
func writeGuardCase(b *bytes.Buffer, k kind, name, args, want, assign string) {
	if k.many() {
		fmt.Fprintf(b, "\t\tpos, err %s mf.%s(%s)\n", assign, name, args)
		fmt.Fprintf(b, "\t\trequire.ErrorIs(t, err, %s)\n", want)
		b.WriteString("\t\trequire.Nil(t, pos)\n")

		return
	}

	if k.Method == "FindRange" {
		fmt.Fprintf(b, "\t\tret, first, last, err %s mf.%s(%s)\n", assign, name, args)
		fmt.Fprintf(b, "\t\trequire.ErrorIs(t, err, %s)\n", want)
		b.WriteString("\t\trequire.Equal(t, uint64(0), ret)\n")
		// The range is empty, so a caller that ignores the error reads nothing.
		b.WriteString("\t\trequire.Equal(t, first, last)\n")

		return
	}

	if k.find() {
		fmt.Fprintf(b, "\t\tret, first, last, err %s mf.%s(%s)\n", assign, name, args)
		fmt.Fprintf(b, "\t\trequire.ErrorIs(t, err, %s)\n", want)
		b.WriteString("\t\trequire.Equal(t, uint64(1), ret)\n")
		b.WriteString("\t\trequire.Equal(t, uint64(0), first)\n")
		b.WriteString("\t\trequire.Equal(t, uint64(1), last)\n")

		return
	}

	fmt.Fprintf(b, "\t\tret, pos, err %s mf.%s(%s)\n", assign, name, args)
	fmt.Fprintf(b, "\t\trequire.ErrorIs(t, err, %s)\n", want)
	b.WriteString("\t\trequire.False(t, ret)\n")

	if k.Method == "HasPrev" {
		b.WriteString("\t\trequire.Equal(t, uint64(1), pos)\n")
		return
	}

	b.WriteString("\t\trequire.Equal(t, uint64(0), pos)\n")
}

// writeMethod appends one generated method to b.
func writeMethod(b *bytes.Buffer, k kind, o order, w width) {
	fmt.Fprintf(b, "\n%s\nfunc (mf *TMMFile) %s(%s) %s {\n",
		k.doc(o, w), k.goName(o, w), k.params(w), k.results())

	if k.Sub {
		fmt.Fprintf(b, "\tif err := checkBits(bitstart, bitend, %d); err != nil {\n\t\t%s\n\t}\n\n", w.Bytes, k.invalid())
	}

	if k.Col {
		fmt.Fprintf(b, "\tsrc, err := mf.colSrc(offset, %s, %d)\n", k.bound(), w.Bytes)
	} else {
		fmt.Fprintf(b, "\tsrc, err := mf.rowSrc(offset, blklen, blkpos, %s, %d)\n", k.bound(), w.Bytes)
	}

	fmt.Fprintf(b, "\tif err != nil {\n\t\t%s\n\t}\n\n", k.invalid())

	if k.many() {
		b.WriteString("\tpos := make([]uint64, len(search))\n")
		b.WriteString("\tif len(search) == 0 {\n\t\treturn pos, nil\n\t}\n\n")
		fmt.Fprintf(b, "\tC.%s(%s)\n\n", k.cName(o, w), k.cargs(w))
		b.WriteString("\treturn pos, nil\n}\n")

		return
	}

	if k.Method == "FindRange" {
		fmt.Fprintf(b, "\tcfirst := C.uint64_t(first)\n\tclast := C.uint64_t(last)\n")
		fmt.Fprintf(b, "\tret := uint64(C.%s(%s))\n\n", k.cName(o, w), k.cargs(w))
		b.WriteString("\treturn ret, uint64(cfirst), uint64(clast), nil\n}\n")

		return
	}

	if k.find() {
		fmt.Fprintf(b, "\tcfirst := C.uint64_t(first)\n\tclast := C.uint64_t(last)\n")
		fmt.Fprintf(b, "\tret := uint64(C.%s(%s))\n\n", k.cName(o, w), k.cargs(w))
		b.WriteString("\treturn ret, uint64(cfirst), uint64(clast), nil\n}\n")

		return
	}

	fmt.Fprintf(b, "\tcpos := C.uint64_t(pos)\n")
	fmt.Fprintf(b, "\tret := bool(C.%s(%s))\n\n", k.cName(o, w), k.cargs(w))
	b.WriteString("\treturn ret, uint64(cpos), nil\n}\n")
}

// subBits returns the bit range the fixtures are searched with for the given
// width. It is the one the FindFirstSub and FindLastSub cases use, because the
// expected results in the tables are the ones of that range.
func subBits(w width) (uint8, uint8) {
	switch w.Bits {
	case 8:
		return 0, 7
	case 16:
		return 0, 15
	case 32:
		return 8, 23
	}

	return 16, 47
}

// colIndex returns the index of the column of the fixture that holds values of
// the given width.
func colIndex(w width) int {
	switch w.Bits {
	case 8:
		return 0
	case 16:
		return 1
	case 32:
		return 2
	}

	return 3
}

// fixture returns the receiver, the table of cases and the arguments that
// describe the layout, for one instantiation.
func fixture(k kind, o order, w width) (recv, table, head string) {
	if k.Col {
		recv = "cmf"
		if o.Go == "BE" {
			recv = "bcmf"
		}

		table = fmt.Sprintf("testDataCol%d", w.Bits)
		if k.Sub {
			table = fmt.Sprintf("testDataColSub%d", w.Bits)
		}

		head = fmt.Sprintf("%s.Index[%d]", recv, colIndex(w))

		if k.Sub {
			start, end := subBits(w)
			head += fmt.Sprintf(", %d, %d", start, end)
		}

		return recv, table, head
	}

	recv = "mf"
	table = fmt.Sprintf("testData%s%d", o.Go, w.Bits)

	if k.Sub {
		table = fmt.Sprintf("testData%sSub%d", o.Go, w.Bits)
	}

	head = "0, 16, tt.blkpos"
	if k.Sub {
		start, end := subBits(w)
		head += fmt.Sprintf(", %d, %d", start, end)
	}

	return recv, table, head
}

// ranges returns a test that checks every FindRange and FindMany method against
// the cases the matching FindFirst and FindLast methods are checked against.
func ranges() []byte {
	var b bytes.Buffer

	b.WriteString(`// Code generated by "make generate"; DO NOT EDIT.

package binsearch

import (
	"testing"

	"github.com/stretchr/testify/require"
)

// TestFindRangeCases checks that every FindRange method reports the half-open
// range that the matching FindFirst and FindLast methods delimit, over the
// cases they are themselves checked against.
func TestFindRangeCases(t *testing.T) {
	t.Parallel()
`)

	for _, k := range kinds {
		if k.Method != "FindRange" {
			continue
		}

		for _, o := range orders {
			for _, w := range widths {
				writeRangeCase(&b, k, o, w)
			}
		}
	}

	b.WriteString(`}

// TestFindManyCases checks that every FindMany method reports, for each value
// of the batch, what the matching FindFirst method reports for it.
func TestFindManyCases(t *testing.T) {
	t.Parallel()
`)

	for _, k := range kinds {
		if k.Method != "FindMany" {
			continue
		}

		for _, o := range orders {
			for _, w := range widths {
				writeManyCase(&b, k, o, w)
			}
		}
	}

	b.WriteString("}\n")

	return b.Bytes()
}

// writeRangeCase appends the checks of one FindRange method to b.
func writeRangeCase(b *bytes.Buffer, k kind, o order, w width) {
	recv, table, head := fixture(k, o, w)
	name := k.goName(o, w)

	fmt.Fprintf(b, `
	t.Run(%q, func(t *testing.T) {
		t.Parallel()

		for _, tt := range %s {
			// The table holds the item the two searches found; the range is
			// empty when the value is absent.
			want := uint64(0)
			if tt.foundFirst != tt.last {
				want = (tt.foundLast - tt.foundFirst) + 1
			}

			n, first, last, err := %s.%s(%s, tt.first, tt.last, tt.search)
			require.NoError(t, err)
			require.Equal(t, want, n, "search 0x%%x", tt.search)

			if n > 0 {
				require.Equal(t, tt.foundFLast, first, "search 0x%%x", tt.search)
				require.Equal(t, tt.foundLLast, last, "search 0x%%x", tt.search)
			}
		}
	})
`, name, table, recv, name, head)
}

// writeManyCase appends the checks of one FindMany method to b.
func writeManyCase(b *bytes.Buffer, k kind, o order, w width) {
	recv, table, head := fixture(k, o, w)
	name := k.goName(o, w)
	// The matching single-value search, used as the reference.
	one := strings.Replace(name, "FindMany", "FindFirst", 1)

	fmt.Fprintf(b, `
	t.Run(%q, func(t *testing.T) {
		t.Parallel()

		// One batch holding every searched value of the table, which is more
		// than two batches wide and leaves a remainder.
		tt := %s[0]
		search := make([]%s, 0, len(%s))

		for _, c := range %s {
			search = append(search, c.search)
		}

		pos, err := %s.%s(%s, tt.first, tt.last, search)
		require.NoError(t, err)
		require.Len(t, pos, len(search))

		for i, v := range search {
			want, _, _, err := %s.%s(%s, tt.first, tt.last, v)
			require.NoError(t, err)
			require.Equal(t, want, pos[i], "value 0x%%x", v)
		}

		// An empty batch reports nothing and reads nothing.
		pos, err = %s.%s(%s, tt.first, tt.last, nil)
		require.NoError(t, err)
		require.Empty(t, pos)
	})
`, name, table, w.Go, table, table, recv, name, head, recv, one, head, recv, name, head)
}
