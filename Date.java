package date;
/**
 *************************************************************************
 *  Simple DATE class that handles: <br>                                     
 * - Days between two dates   <br>                                           
 * - Validate a set of number whether it is a valid date    <br>             
 * - Set and get methods to retrieve date information <br>                   
 *************************************************************************/

public class Date {
    private static final int[] DAYS = { 0, 31, 29, 31, 30, 31, 30, 31, 31,
			30, 31, 30, 31 };
    private static final int LEAPYEAR = 366;

	private int month; // month (between 1 and 12)
	private int day; // day (between 1 and day of the month)
	private int year; // year from 0-999

    /**
	 * Constructor: create an empty object if date is not valid
	 * @param mm    value for months, from 1 to 12
	 * @param dd    day in the given month, within the limits of that month
	 * @param yyyy  year, take a year, values from 0-999 <br>
    */ 
    public Date(int mm, int dd, int yyyy) {
        if(isValid(mm,dd,yyyy)){
        month = mm;
	day = dd;
	year = yyyy;
        }
	}
    /**
     * Constructor: create an empty object
     */
    public Date() {
    }
    
    /**
     * Constructor: create a Date object with value given by the string
     * @param dateinString
     */
    public Date(String dateinString) {
		int mm, dd, yyyy;
		String[] chopDate = dateinString.split("-");
		mm = Integer.parseInt(chopDate[0]);
		dd = Integer.parseInt(chopDate[1]);
		yyyy = Integer.parseInt(chopDate[2]);
		if (isValid(mm, dd, yyyy)){
		month = mm;
		day = dd;
		year = yyyy;
        }
	}    

    /** 
	 * @param mm    value for months, from 1 to 12
	 * @param dd    day in the given month, within the limits of that month
	 * @param yyyy  year, take a year, values from 0-999 <br>
     */
    public void setDate(int mm, int dd, int yyyy){
        if(isValid(mm,dd,yyyy)){
        month = mm;
	day = dd;
	year = yyyy; 
        }
    }

    /**
     
     * @return Month value
     */
    public int getMonth(){
        return month;
    }

    /**
     * Change the month value to m
     * @param m 
     */
    public void setMonth(int m){
        month = m;
    }

    /**
     *
     * @return Day value of the object
     */
    public int getDay(){
        return day;
    }

    /**
     * Set day value to d
     * @param d
     */
    public void setDay(int d){
        day = d;
    }

    /**
     * Set year value to y
     * @param y
     */
    public void setYear(int y){
        year = y;
    }

    /**
     * 
     * @return year value of the date object
     */
    public int getYear(){
        return year;
    }
    /**
	 * Is the given date valid?
     * @param m :  month cannot exceed 12 and less than 1
     * @param d : cannot exceed the number of days a month should have, 
         * and less than 1
     * @param y: year is not a leap year and has 29 days
         * or if year is not within 0-9999 range
     * @return: true if year is valid, false otherwise 
	 */
    public static boolean isValid(int m, int d, int y) {
            return ((y > 0 && y< 9999) 
                && (m >= 1 && m <= 12) 
                && (d >= 1 && d <= DAYS[m])
                && !(m == 2 && d == DAYS[2] && !isLeapYear(y)));
	}

	/**
	 * is y a leap year?
     * return true if year divisible by 400
     * or if the year is divisible by 4 and not 100
     * all else return false
     */
    private static boolean isLeapYear(int y) {
        return y % 400 == 0 || (y%4==0 && y%100!=0);
    }
    
    /**
    * compares two date objects
    *
    * @param b Date object
    * @return true if this Date is before Date b
    */
    private boolean isBefore(Date b) {
        return compareTo(b) < 0;
    }
    
    /**
     * compares two Date objects
     *
     * @param b    Date object
     * @return     0 if this Date is the same as Date b <br>
     *             negative integer if this Date is earlier than Date b <br>
     *             positive integer if this Date is after Date b
     */
    private int compareTo(Date b) {
        if (year != b.year)
            return year - b.year;
        if (month != b.month)
            return month - b.month;
        return day - b.day;
    }
    
    /**
     *
     * @param d: a date object to be compared with current object
     * @return days in between two dates
     */
    public int daysBetween(Date d) {
        Date startDate;
        Date endDate;
        if (!(d.isBefore(this))){
            startDate = this;
            endDate = d;
        } else{
            startDate = d;
            endDate = this;
        }
        int numDays = 0;
        //Sum up the days from 1/1/startyear - 12/31/endyear
        for(int i = startDate.year;i<=endDate.year;i++)
            numDays+= isLeapYear(i) ? 366 : 365;
        //subtract number of days from Jan - startmonth (if Jan, then -0)
        for(int i = 1;i<startDate.month;i++)
            numDays-= (i == 2 && !isLeapYear(startDate.year)) ? DAYS[i]-1: DAYS[i];
        //subtract number of days from endMonth - December 
        for(int i = endDate.month;i<12;i++)
            numDays-= (i == 2 && !isLeapYear(endDate.year)) ? DAYS[i-1] : DAYS[i] ;
        //substract the past days of the start month and the remaining days of the end month, -1
        numDays = numDays-startDate.day-(DAYS[endDate.month]-endDate.day)-1;
        return numDays;
    }
    
    /**
     * 
     * @return
     */
    public String dateToString() {
		return "[" + month + "-" + day + "-" + year + "]";
	}

    /**
     *
     * @param args
     */
    public static void main(String[] args) {
            
        //Testing the isValid method
            int dataArray [][] ={{13,01,2010},
                                 {03,31,2011},
                                 {04,31,2012},
                                 {2,29,2016},
                                 {2,29,1500},
                                 {2,29,1600}};
            for(int i = 0;i<6;i++){
                Date test = new Date(dataArray[i][0],dataArray[i][1], dataArray[i][2]);
                System.out.printf("Is %02d-%02d-%4d a valid date? : %b \n", dataArray[i][0],dataArray[i][1], dataArray[i][2], 
                                                               isValid(dataArray[i][0],dataArray[i][1], dataArray[i][2]));
            }
            
        //Testing Daysbetween method
            String startDates [] = {"1-1-2013", "12-31-2019", "12-31-2016", "1-1-1700", "1-1-2000", "2-12-2000"};
            String endDates [] = {"12-30-2013","1-1-2020","1-1-2016","3-1-1700","3-1-2000","5-23-2005"};
            
            for(int i=0;i<6;i++){
                Date startDate = new Date(startDates[i]);
                Date endDate = new Date(endDates[i]);
                int daysbetween = startDate.daysBetween(endDate);
                System.out.printf("Days between two dates: %s and %s is %d \n", startDate.dateToString(), endDate.dateToString(), daysbetween );
            }
    }
}
