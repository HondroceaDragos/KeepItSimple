package main

import (
	"fmt"
	"log"
	"os"

	"github.com/muesli/termenv"
)

func main() {
	outputStream, err := os.Create("./data/terminal.txt")
	if err != nil {
		log.Fatal(err)
	}

	defer outputStream.Close()

	rc, _ := termenv.EnableVirtualTerminalProcessing(termenv.NewOutput(os.Stdout))
	defer rc()

	bg := termenv.BackgroundColor()
	fg := termenv.ForegroundColor()

	bgHex := termenv.ConvertToRGB(bg).Hex()
	fgHex := termenv.ConvertToRGB(fg).Hex()

	fmt.Fprintf(outputStream, "bg %s\nfg %s\n", bgHex, fgHex)
}
