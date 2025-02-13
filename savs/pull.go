package main

import (
	"fmt"
	"io"
	"os"
	"strings"
)

const offset int64 = 0x0001E008
const bLength int = 4000
const results string = "results.txt"

func main() {

	la := len(os.Args)

	if la < 2 || la > 2 {
		fmt.Println("No input file specified!")
		os.Exit(1)
	}

	sArg := os.Args[1]
	dat, err := getSaveDataAtOffset(sArg, offset, bLength)
	checkError(err)

	writeSeedList(
		makeSeedList(bLength, dat),
		results,
	)
}

func getSaveDataAtOffset(path string, offset int64, l int) ([]byte, error) {
	f, err := os.Open(path)

	if err != nil {
		return nil, err
	}

	defer f.Close()

	buf := make([]byte, l)
	_, err = f.Seek(offset, io.SeekStart)

	if err != nil {
		return nil, err
	}

	_, err = f.Read(buf)

	if err != nil {
		return nil, err
	}

	return buf, err
}

func writeSeedList(s []string, p string) error {
	data := strings.Join(s, "\n")
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
		if s[0] == 0 && s[1] == 0 {
			continue
		} else {
			out = append(out, fmt.Sprintf("'%02X%02X", s[1], s[0]))

		}
	}
	return out
}

func printSeedList(ls []string) {
	for _, o := range ls {
		fmt.Println(o)
	}
}

func checkError(e error) {
	if e != nil {
		panic(e)
	}
}
