package main

import (
	"io"
	"os"
	"strings"
	"fmt"
)

func StrToC(inp string) string {
	out := ""
	split := strings.Split(inp, "\n")

	for i:=0; i < len(split); i++ {
		tmp := strings.ReplaceAll(split[i], "\\", "\\\\")
		out += "\"" + strings.ReplaceAll(tmp, "\"", "\\\"") + "\\n\"\n"
	}

	return out
}

func ReadText(path string) string {
	f, err := os.Open(path)
	if err != nil {
		fmt.Println(err.Error())
		return ""
	}

	dat, err := io.ReadAll(f)
	if err != nil {
		fmt.Println(err.Error())
		return ""
	}

	return string(dat)
}
