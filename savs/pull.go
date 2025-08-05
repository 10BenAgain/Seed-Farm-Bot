package main

import (
	"encoding/binary"
	"fmt"
	"io"
	"os"
	"strings"
)

const (
	Offset int64 = 0x0001E000
	Count  int64 = 0x4
	Start  int64 = 0x6

	BlockLength int = 4000
)

var (
	results string = "results.txt"
)

func main() {

	la := len(os.Args)

	if la < 2 || la > 2 {
		fmt.Println("No input file specified!")
		os.Exit(1)
	}

	seeds := makeSeedList(getSaveDataAtOffset(os.Args[1]))
	writeSeedList(seeds, results)
}

func getSaveDataAtOffset(path string) ([]byte, int) {
	f, err := os.Open(path)

	if err != nil {
		panic(err)
	}
	defer f.Close()

	_, err = f.Seek(Offset, io.SeekStart)
	if err != nil {
		panic(err)
	}

	buf := make([]byte, Offset+Start)
	_, err = f.Read(buf)
	if err != nil {
		panic(err)
	}

	size := binary.LittleEndian.Uint16(buf[Count:Start])
	return buf, int(size)
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

func makeSeedList(buf []byte, l int) []string {

	out := []string{}
	for i := int(Start); i <= (l*2)+2; i += 2 {
		n := binary.LittleEndian.Uint16(buf[i : i+2])
		out = append(out, fmt.Sprintf("%04X", n))
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
