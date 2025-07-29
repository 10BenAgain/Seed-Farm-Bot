package main

import (
	"encoding/csv"
	"fmt"
	"net/http"
	"os"
	"slices"
	"strconv"
	"sync"

	"github.com/charmbracelet/lipgloss"
	"github.com/charmbracelet/lipgloss/table"
)

const (
	GreatestDifference = 161
	FireRedSheetId     = "1ZNchTvoCpHFVPBscEJZG3JaaqR41D8VVnbXb23fzc44"
	FireRedSheetGid    = "0"
)

var (
	AButtonCols     = []int{3, 4, 5, 6, 7} // and 8
	StartButtonCols = []int{9, 10, 11, 12, 13, 14}
	LButtonCols     = []int{15, 16}
)

// https://lospec.com/palette-list/teaviie
var (
	LightPink   = lipgloss.Color("#f6e1ff")
	MidPink     = lipgloss.Color("#f6bafb")
	DarkPink    = lipgloss.Color("#e26cd7")
	LightPurple = lipgloss.Color("#9729fc")
	MidPurple   = lipgloss.Color("#6420a8")
	DarkPurple  = lipgloss.Color("#11002a")

	headerStyle = lipgloss.NewStyle().Foreground(MidPink).Bold(true).Align(lipgloss.Center)
	cellStyle   = lipgloss.NewStyle().Padding(0, 1).Width(14).Align(lipgloss.Center)
	oddRowStyle = cellStyle.Foreground(DarkPink)
	evenRowStle = cellStyle.Foreground(LightPurple)

	countStyle = lipgloss.NewStyle().Foreground(MidPink)
)

func main() {
	var wg sync.WaitGroup

	gt := func() ([][]string, error) {
		resp, err := pullSheetData(FireRedSheetId, FireRedSheetGid)
		if err != nil {
			return nil, err
		}

		data, err := getSheetDataFromResp(resp)
		if err != nil {
			return nil, err
		}

		return data, err
	}

	wg.Add(1)
	go func() {
		defer wg.Done()

		if data, err := gt(); err != nil {
			panic(err)
		} else {
			fmt.Println("Results for A Button Sanity Checks (Cols D-I)")
			printResultTable(data, AButtonCols)
		}
	}()

	wg.Add(1)
	go func() {
		defer wg.Done()

		if data, err := gt(); err != nil {
			panic(err)
		} else {
			fmt.Println("Results for Start Button Sanity Checks (Cols J-O)")
			printResultTable(data, StartButtonCols)
		}
	}()

	wg.Add(1)
	go func() {
		defer wg.Done()

		if data, err := gt(); err != nil {
			panic(err)
		} else {
			fmt.Println("Results for Start Button Sanity Checks (Cols P-Q)")
			printResultTable(data, LButtonCols)
		}
	}()

	wg.Wait()
	fmt.Println("Done")
}

func makeResultsTable(results [][]string) *table.Table {
	return table.New().
		Border(lipgloss.NormalBorder()).
		BorderStyle(lipgloss.NewStyle().Foreground(MidPink)).
		StyleFunc(func(row, col int) lipgloss.Style {
			switch {
			case row == table.HeaderRow:
				return headerStyle
			case row%2 == 0:
				return evenRowStle
			default:
				return oddRowStyle
			}
		}).
		Headers("#", "LINE", "MAX", "MIN", "DIFF").
		Rows(results...)
}

func sanityCheck(data [][]string, cols []int) ([][]string, int) {
	var results [][]string
	t := make([]uint64, len(cols))

	tlen := len(t)
	count := 0
	for _, row := range data {

		// Convert each value to a uint to calculate difference
		{
			for i, val := range cols {
				if num, err := strconv.ParseUint(row[val], 16, 32); err != nil {
					t[i] = 0
				} else {
					t[i] = num
				}
			}
		}

		// Since the array length is always going to be small,
		// this will probably be the most efficient sort
		slices.Sort(t)

		diff := t[tlen-1] - t[0]
		if diff > GreatestDifference {
			count++
			results = append(
				results,
				[]string{
					fmt.Sprint(count),
					row[0],
					fmt.Sprint(t[tlen-1]),
					fmt.Sprint(t[0]),
					fmt.Sprint(t[tlen-1] - t[0]),
				},
			)
		}
	}

	return results, count
}

func pullSheetData(id, gid string) (*http.Response, error) {
	link := fmt.Sprintf("https://docs.google.com/spreadsheets/d/%s/export?format=csv&gid=%s", id, gid)
	if resp, err := http.Get(link); err != nil {
		return nil, err
	} else {
		return resp, nil
	}
}

func getSheetDataFromResp(resp *http.Response) ([][]string, error) {
	reader := csv.NewReader(resp.Body)
	if data, err := reader.ReadAll(); err != nil {
		return nil, err
	} else {
		return data, nil
	}
}

func printResultTable(data [][]string, cols []int) {
	results, count := sanityCheck(data, cols)
	if count > 0 {
		fmt.Println(makeResultsTable(results).String())
		fmt.Println(countStyle.Render(fmt.Sprintf("Sanity checks failed: %d", count)))
	} else {
		fmt.Println("All sanity checks passed!")
	}
}

func getSheetDataFromFile(path string) ([][]string, error) {
	if file, err := os.Open(path); err != nil {
		return nil, err
	} else {
		if data, err := csv.NewReader(file).ReadAll(); err != nil {
			return nil, err
		} else {
			return data, nil
		}
	}
}

func printTestResultTable() {
	if data, err := getSheetDataFromFile("test.csv"); err != nil {
		panic(err)
	} else {
		printResultTable(data, AButtonCols)
	}
}
