// Build tools for the binsearch repository, declared in a separate module so that
// they stay out of the module graph of anything importing binsearch.
// Installed into target/binutil by "make gotools".
module github.com/tecnickcom/binsearch/go/resources/tools

go 1.24

tool github.com/jstemmer/go-junit-report/v2

require github.com/jstemmer/go-junit-report/v2 v2.1.0 // indirect
