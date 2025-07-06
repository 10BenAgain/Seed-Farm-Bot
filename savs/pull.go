package main

import (
	"fmt"
	"io"
	"os"
	"strings"
)

const (
	Offset      int64 = 0x0001E006
	BlockLength int   = 4000
)

var results string = "results.txt"

func main() {

	la := len(os.Args)

	if la < 2 || la > 2 {
		fmt.Println("No input file specified!")
		os.Exit(1)
	}

	sArg := os.Args[1]

	dat, err := getSaveDataAtOffset(sArg, Offset, BlockLength)
	checkError(err)

	err = writeSeedList(
		makeSeedList(BlockLength, dat),
		results,
	)

	checkError(err)

	// clearSeedBlock("FR_STEREO_LA_A_3500_3540.sav")
}

func getSaveDataAtOffset(path string, Offset int64, l int) ([]byte, error) {
	f, err := os.Open(path)

	if err != nil {
		return nil, err
	}

	defer f.Close()

	buf := make([]byte, l)
	_, err = f.Seek(Offset, io.SeekStart)

	if err != nil {
		return nil, err
	}

	_, err = f.Read(buf)

	if err != nil {
		return nil, err
	}

	return buf, err
}

func clearSeedBlock(p string) {
	f, err := os.OpenFile(p, os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		panic(err)
	}

	defer f.Close()

	blank := make([]byte, BlockLength)

	_, err = f.WriteAt(blank, Offset-6)
	if err != nil {
		panic(err)
	}
}

func writeSeedList(s []string, p string) error {
	data := strings.Join(s, "\n")

	if isFileExist(results) {
		if err := os.Remove(results); err != nil {
			return err
		}
	}

	err := os.WriteFile(p, []byte(data), 0644)
	return err
}

func makeSeedList(l int, buf []byte) []string {
	if l%2 != 0 {
		return nil
	}

	out := []string{}
	s := make([]byte, 2)
	for i := 0; i < l-2; i += 2 {
		s = buf[i : i+2]
		if !(s[0] == 0 && s[1] == 0) {
			out = append(out, fmt.Sprintf("'%02X%02X", s[1], s[0]))
		}
	}
	return out
}

func isFileExist(p string) bool {
	if fi, err := os.Stat(p); os.IsNotExist(err) {
		return false
	} else {
		return !fi.IsDir()
	}
}

func checkError(e error) {
	if e != nil {
		panic(e)
	}
}
