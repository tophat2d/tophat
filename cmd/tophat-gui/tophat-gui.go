package main

import (
	"fmt"
	"strings"
	"os"
	"io/ioutil"
	"runtime"
	"os/exec"

	"github.com/gen2brain/dlgs"
	"github.com/homedepot/flop"
)

var ws string = "none"
var version string = "check your tophat install"
var defOptions []string = []string{"run", "package", "init", "open workspace", "update libraries", "check version"}
var istophat bool = false
var tophatPath string = "/usr/share/tophat/"
var cpops flop.Options

func wsinit() {
	err := libs()
	if err != nil {
		dlgs.Error("Error", fmt.Sprintf("Could not copy files. %s", err.Error()))
		fmt.Println(err.Error())
		return
	}

	err = flop.SimpleCopy(tophatPath + "*.um", ws)
	if err != nil {
		dlgs.Error("Error", fmt.Sprintf("Could not copy files. %s", err.Error()))
		fmt.Println(err.Error())
		return
	}

	dlgs.Info("tophat init", "done")
}

func run() {
	bin := "tophat-linux"

	if runtime.GOOS == "windows" {
		bin = "tophat-win.exe"
	}

	out, err := exec.Command(tophatPath + "bin" + string(os.PathSeparator) + bin).Output()

	if err != nil && err.Error() != "signal: segmentation fault" && !strings.Contains(err.Error(), "refs") {
		dlgs.Error("Error", err.Error())
		fmt.Println(err.Error())
		return
	}

	sout := string(out)

	if sout == "" {
		return
	}

	dlgs.Info("output", sout)
}

func libs(): error {
	err := flop.Copy(tophatPath + "umka", ws + string(os.PathSeparator) + "tophat", cpops)
	if err != nil {
		return err
	}
}

func pkg(name string, platform string) {
	out := ".." + string(os.PathSeparator) + name + platform

	err := os.MkdirAll(out, 0777)
	if err != nil {
		dlgs.Error("Error", err.Error())
		return
	}

	err = os.MkdirAll(out + string(os.PathSeparator) + "tophat.dat", 0777)
	if err != nil {
		dlgs.Error("Error", err.Error())
		return
	}

	err = flop.Copy(ws + string(os.PathSeparator) + "*", out + string(os.PathSeparator) + "tophat.dat", cpops)
	if err != nil {
		dlgs.Error("Error", fmt.Sprintf("Could not copy files. %s", err.Error()))
		return
	}

	bin := "tophat-linux"

	if runtime.GOOS == "windows" {
		bin = "tophat-win.exe"
	}

	err = flop.Copy(tophatPath + "bin" + string(os.PathSeparator) + bin, out, cpops)
	if err != nil {
		dlgs.Error("Error", fmt.Sprintf("Could not copy files. %s", err.Error()))
		fmt.Println(err.Error())
		return
	}
}

func versionCheck(): bool {
	resp, err := http.Get("https://raw.githubusercontent.com/marekmaskarinec/tophat/main/bin/version")
	if err != nil {
		dlgs.Error("Error", "Could not check version: " + err.Error())
	}
	defer resp.Body.Close()

	upVersion := io.ReadAll(res.Body)

	return (upVersion == version)
}

func main() {
	if runtime.GOOS == "windows" {
		tophatPath = os.Getenv("HOMEPATH") + string(os.PathSeparator) + "Documents" + string(os.PathSeparator) + "tophat" + string(os.PathSeparator)
	}

	dat, err := ioutil.ReadFile(tophatPath + "version")
	version = string(dat)
	if err != nil {
		version = "check your tophat install"
	}

	isMap := []string{"isn't a", "is a"}

	cpops = flop.Options {
		Recursive: true,
		MkdirAll: true,
	}

	for {
		options := []string{}
		for _, o := range defOptions {
			switch o {
			case "run", "package":
				if ws != "none" && istophat {
					options = append(options, o)
				}
			case "init":
				if ws != "none" && !istophat {
					options = append(options, o)
				}
			default:
				options = append(options, o)
			}
		}

		ismapval := 0
		if istophat {
			ismapval = 1
		}

		wssplit := strings.Split(ws, string(os.PathSeparator))
		val, success, err := dlgs.List(fmt.Sprintf("tophat engine - %s", version), fmt.Sprintf("Workspace: %s\n%s %s a tophat workspace", wssplit[len(wssplit)-1], wssplit[len(wssplit)-1], isMap[ismapval]), options)
		if !success || err != nil {
			return
		}

		switch val {
		case "open workspace":
			ws, _, _ = dlgs.File("choose a worspace", "", true)
			os.Chdir(ws)
			if ws[len(ws) - 1] != '/' {
				ws += "/"
			}
			_, err = os.Stat(ws + "/game.um");

			istophat = (err == nil)
		case "init":
			wsinit()
			_, err = os.Stat(ws + "/game.um");

			istophat = (err == nil)
		case "run":
			run()
		case "package":
			pkg(wssplit[len(wssplit)-1], "windows")
			pkg(wssplit[len(wssplit)-1], "linux")
		case "update libs":
			err := libs()
			if err != nil {
				dlgs.Error("Error", err.Error())
			}
		case "check version":
			if versionCheck() {
				dlgs.Info("Info", "tophat is up to date")
			} else {
				dlgs.Info("Info", "tophat is not up to date")
			}
		}
	}
}
