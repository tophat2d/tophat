package main

import (
	"fmt"
	"os"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("Not enough arguments")
		os.Exit(1)
	}

	switch os.Args[1] {
	case "text":
		if len(os.Args) < 3 {
			fmt.Println("Not enough arguments")
			os.Exit(1)
		}

		for i:=2; i < len(os.Args); i++ {
			fmt.Println(StrToC(ReadText(os.Args[i]))+",")
		}
	}
}

