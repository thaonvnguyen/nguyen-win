/*
Class: CSE 3330
Semester: Fall 2016
Student Name: THAO NGUYEN TXN2617
Student ID: 1001142617
 */

import java.io.IOException;
import java.util.*;
import java.sql.*;
import java.text.SimpleDateFormat;

public class Project4 {

  static final String JDBC = "jdbc:mysql://localhost:3306/txn2617?"
          + "user=txn2617" + "&password=Apple2016";
  private ResultSet rs; //Store query result here
  private Connection con;
  private Statement stmt;
  private String PassengerName;
  private String PassengerPhone;
  private String FlightNo;
  private String FlightDate;
  private String fromA;
  private String toA;
  private String seatType;

  public Project4() { //Cpnstructor for class
  }

  /*
  This method prompts user for all the necessary input for a new reservation.
  It will also check the validity of the input before processing.
   */
  public void reservationInput() throws SQLException {
    Scanner sc = new Scanner(System.in);
    String Flight_Query = "select FLNO, FromA, ToA, FDate from "
            + "FlightLeg natural join FlightLegInstance";
    rs = stmt.executeQuery(Flight_Query);
    if (!isEmptyQuery(rs)) {
      System.out.printf("FLNO\tFromA\tToA\tFDate \n\n");
      while (rs.next()) {
        System.out.println(rs.getString("FLNO") + "\t" + rs.getString("FromA") + "\t"
                + rs.getString("ToA") + "\t" + rs.getString("FDate"));
        System.out.println();
      }
    }
    System.out.printf("- Passenger Name?\n");
    PassengerName = sc.next();

    System.out.printf("- Passenger Phone?\n");
    PassengerPhone = sc.next();

    System.out.printf("- Flight Number?\n");
    FlightNo = sc.next();

    System.out.printf("- Flight Date?\n");
    FlightDate = sc.next();

    System.out.printf("-From airport?\n");
    fromA = sc.next();

    System.out.printf("-To airport?\n");
    toA = sc.next();

    System.out.printf("-Seat class?\n");
    seatType = sc.next();
  }

  /*
  This method prompts user for all the necessary input for a new capacity query.
  It will also check the validity of the input before processing.
   */
  public void capacityInput() throws SQLException {
    Scanner sc = new Scanner(System.in);
    String Airport_Query = "SELECT * FROM Airport";
    rs = stmt.executeQuery(Airport_Query);
    if (!isEmptyQuery(rs)) {
      System.out.printf("Code\tCity & State\n\n");
      while (rs.next()) {
        System.out.println(rs.getString("Code") + "\t" + rs.getString("City") + ", "
                + rs.getString("State"));
        System.out.println();
      }
    }
    System.out.printf("-From airport?\n");
    fromA = sc.next();
    System.out.printf("-To airport?\n");
    toA = sc.next();
    System.out.printf("-Seat class?(E for Economy/F for Firstclass)\n");
    seatType = sc.next();
  }
  /*
  This method return the userID from the passenger name and phone that was input
  Method will return -1 if no record found
  */
  public int getUserID() throws SQLException {
    String Customer_Query = "SELECT * FROM Passenger"
            + " WHERE (name = \"" + PassengerName
            + "\" AND phone = \"" + PassengerPhone + "\")";
    rs = stmt.executeQuery(Customer_Query);
    int rowcount = 0;
    if (rs.last()) {
      rowcount = rs.getRow();
      rs.beforeFirst();
    }
    if (rowcount == 0) {
      return -1;
    } else {
      rs.first();
      return rs.getInt("ID");
    }
  }
  /*
  This method check if a passing query is empty, return true if so, false otherwise
  */
  public boolean isEmptyQuery(ResultSet rs) throws SQLException {
    int rowcount = 0;
    if (rs.last()) {
      rowcount = rs.getRow();
      rs.beforeFirst();
    }
    if (rowcount == 0) {
      return true;
    } else
        return false;
  }

  /* Return the last userID in the user table, used to retrieve the largest ID
  before another method increment that to create a new ID
  */
  public int getLastID() throws SQLException {
    String Customer_Query = "SELECT * FROM Passenger";
    rs = stmt.executeQuery(Customer_Query);
    rs.last();
    int lastID = rs.getInt("ID");
    return lastID;
  }
  //Load the JDBC driver
  public static void loadJDBC() {
    try {
      Class.forName("com.mysql.jdbc.Driver").newInstance();
    } catch (Exception ex) {
      System.out.printf("Loading JDBC driver unsuccessfully\n");
    }
  }
  //estabish connection with the database
  public void establishConnection() {
    try {
      con = DriverManager.getConnection(JDBC);
      stmt = con.createStatement();
    } catch (SQLException ex) {
      System.out.printf("Connection with database failed: %s\n", ex);
    }
  }
  //Method to make a new reservation, option 2 in the main function
  public void makeReservation() throws SQLException {
    //TODO check if reservation exists
    reservationInput();
    int customerID = getUserID();
    if (customerID == -1) {
      int newID = getLastID();
      newID++;
      System.out.printf("Customer not in record. Adding customer...\n\n");
      String InsertCustomer = "INSERT INTO Passenger"
              + " values (" + newID + ", '" + PassengerName + "', '" + PassengerPhone + "')";
      stmt.execute(InsertCustomer);
    }
    //Either Customer exists, or has been added above
    customerID = getUserID();
    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
    String dateString = sdf.format(Calendar.getInstance().getTime());
    String InsertReservation = "INSERT INTO Reservation "
            + "values(" + customerID
            + ",\"" + FlightNo
            + "\",\"" + FlightDate
            + "\",\"" + fromA
            + "\",\"" + toA
            + "\",\"" + seatType
            + "\",\"" + dateString
            + "\", NULL)";
    try {
      stmt.execute(InsertReservation);
      System.out.printf("New reservation added...\n\n");
    } catch (SQLException ex) {
      System.out.printf("Unable to add this reservation. Check information and try again");
    }
  }

  //Query method for airport to airport capacity
  public void airportToAirportCapacity() throws SQLException {
    capacityInput();
    String Airport_To_Airport_Capacity_Query = "SELECT * FROM V3"
            + " WHERE (FromA = \"" + fromA + "\" AND ToA = \"" + toA + "\" "
            + "AND SeatType = \"" + seatType + "\")";
    rs = stmt.executeQuery(Airport_To_Airport_Capacity_Query);

    if (!isEmptyQuery(rs)) {
      while (rs.next()) {
        System.out.println(rs.getString("FromA") + " " + rs.getString("ToA") + " "
                + rs.getString("SeatType") + " " + rs.getString("AvailableSeats"));
        System.out.println();
      }
    } else {
      System.out.printf("No such record\n\n");
    }
  }
  //Hidden method to show tables, using input menu option 999
  public void showTable() throws SQLException {
    Scanner sc = new Scanner(System.in);
    System.out.printf("Which table to show? ");
    String tableName = sc.next();
    String Table_Query = "SELECT * FROM " + tableName;
    try {
      rs = stmt.executeQuery(Table_Query);
    } catch (SQLException ex) {
      System.out.printf("Cannot find that table\n");
      return;
    }
    if (!isEmptyQuery(rs)) {
      ResultSetMetaData rsmd = rs.getMetaData();

      int columnCount = rs.getMetaData().getColumnCount();
      for (int i = 1; i <= columnCount; i++) {
        String name = rsmd.getColumnName(i);
        System.out.printf("%s  ", name);
      }
      System.out.println();
      while (rs.next()) {
        StringBuilder builder = new StringBuilder();
        //int columnCount = rs.getMetaData().getColumnCount();
        for (int i = 0; i < columnCount;) {
          builder.append(rs.getString(i + 1));
          if (++i < columnCount) {
            builder.append("  ");
          }
        }
        builder.append("\r\n");
        System.out.println(builder.toString());
        System.out.println();
      }
    } else {
      System.out.printf("Cannot find that table");
    }
  }

  public static void main(String[] args) throws IOException {
    Project4 project = new Project4();
    loadJDBC();
    project.establishConnection();
    String choice = "";
    // Option menu
    while (!choice.equalsIgnoreCase("/q")) {
      System.out.printf("Select from the following menu: \n");
      System.out.printf("-- Display capacity from one airport to another - Press 1 \n");
      System.out.printf("-- Make a reservation - Press 2\n");
      System.out.printf("-- Enter /q to quit (or press Ctl + C at any point) \n");
      //switch between two functions
      Scanner sc = new Scanner(System.in);
      choice = sc.next();
      System.out.print("\033[H\033[2J");
      try {
        if (choice.equals("1")) {
          project.airportToAirportCapacity();
        } else if (choice.equals("2")) {
          project.makeReservation();
        } else if (choice.equals("999")) {
          project.showTable();
        }
      } catch (SQLException ex) {
        System.out.printf("Operation failed. Check input and try again\n");
      }
    }
    try {
      project.rs.close();
      project.stmt.close();
    } catch (SQLException e) {
      }
    System.out.printf("Program exiting..Bye \n");
  }
}
