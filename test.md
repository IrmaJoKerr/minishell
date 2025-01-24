# Case Study - Advertising Campaign Launch Date

## 📚 Table of Contents
- [Back to Excel Basics](#back-to-excel-basics)
- [Task Summary](#task-summary)
- [Output](#output)
- [Solution](#solution)

### Back to Excel Basics:
- Click to go **[back to Excel Basics](https://github.com/nacht29/microsoft-power-bi-professional-cert/tree/main/excel-basics)**

### Task Summary
Adventure Works is preparing a series of advertising campaigns to be rolled out in several different regions.
Update the spreadsheet that focuses on the launch dates for the USA campaign.

Include the following information:

- The number of working days available between the start date and the deadline date.
- The month and year when each campaign will launch.
- The number of calendar days to the deadline date for each campaign.

### Skills
- Data standardisation
- DATETIME Function

### Output:

![image](https://github.com/user-attachments/assets/6633f6c4-2363-4b65-be6d-347a7614aa75)

### Solution

**Steps:**

1. Formatting data
    - For readability, format the date to ```dd/mm/yyyy```.

    - Select ```C5:D5```, go to ```Home > Number```. Select the drop down box (it should likely show "General")  Select ```More Number Formats > Date```. In the ```Type``` option, select the format with ```dd/mm/yyyy```. Example: ```*14/3/2012```.

    - The original data is from the year 2023. For this project's purpose, we update the year to 2025 and onwards. 

    - Use ```=DATE(YEAR(C5)+2,MONTH(C5),DAY(C5))``` and ```DATE(YEAR(D5)+2,MONTH(D5),DAY(D5))``` on ```E5``` and ```F5``` respectively to increment the original date data by 2. Select ```E5:F5``` and use auto-fill to complete the data for columns E and F.

    - Select the data in columns E and F. Use ```Ctrl + C``` to copy the values in both columns. Use auto-fill to select the cells containing data in columns C and D then use ```Ctrl + Alt + V``` to paste the values copied without copying over the formula. Delete the data in columns E and F.

2. Creating the report
    - Use ```=TODAY()``` in ```B1``` to obtain the current date according to the computer's calendar.

    - In ```E5```, use ```=D5-$B$1``` to calculate the number of days between the current date to the project's launch date. Note the absolute cell reference ```$B$1``` as we want to ensure the current date is constant for the calculation in the cells in ```Calendar Days to launch``` column.

    - Use ```S=NETWORKDAY($B$1,D5,$J$5:$J$26)``` on ```F5``` to calculate the number of working days from the current date to the launch date. the ```NETWORKDAY``` function excludes holidays listed in ```J5:J26``` and also weekends. Note the absolute cell reference ```$J$5:$J$26``` to keep the holiday dates constant in the calculations.

#### Feel free to download the Excel file, ````Case Study - Advertising Campaign USA Dates.xlsx```` for a better view of the end result.
