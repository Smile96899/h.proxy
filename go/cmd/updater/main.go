package main

import (
	"io/ioutil"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
	"time"
)

func main() {

	exe, err := os.Executable()
	if err != nil {
		panic(err.Error())
	}

	wd := filepath.Dir(exe)
	os.Chdir(wd)
	exe = filepath.Base(os.Args[0])
	log.Println("exe:", exe, "exe dir:", wd)

	if strings.HasPrefix(strings.ToLower(exe), "updater") {
		if runtime.GOOS == "windows" {
			if strings.HasPrefix(strings.ToLower(exe), "updater.old") {

				time.Sleep(time.Second)
				Updater()

				exec.Command("./nekobox.exe").Start()
			} else {

				Copy("./updater.exe", "./updater.old")
				exec.Command("./updater.old", os.Args[1:]...).Start()
			}
		} else {

			Updater()

			if os.Getenv("NKR_FROM_LAUNCHER") == "1" {
				Launcher()
			} else {
				exec.Command("./nekobox").Start()
			}
		}
		return
	} else if strings.HasPrefix(strings.ToLower(exe), "launcher") {
		Launcher()
		return
	}
	log.Fatalf("wrong name")
}

func Copy(src string, dst string) {

	data, _ := ioutil.ReadFile(src)

	ioutil.WriteFile(dst, data, 0644)
}
