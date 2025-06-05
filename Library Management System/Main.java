package IOOM_LAB.ASSIGNMENT6_3;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Library library = new Library();

        Scanner sc = new Scanner(System.in);
        boolean exit = false;
        Librarian librarian = new Librarian("L001", "Head Librarian");
        librarian.addBook(library, new Book("B001", "Java Programming", "John Doe"), 5);
        librarian.addBook(library, new Book("B002", "Data Structures and Algorithms", "Jane Smith"), 3);
        librarian.addBook(library, new Book("B003", "Design Patterns", "Erich Gamma"), 4);
        librarian.addBook(library, new Book("B004", "Clean Code", "Robert C. Martin"), 6);
        librarian.addBook(library, new Book("B005", "Effective Java", "Joshua Bloch"), 2);
        librarian.addBook(library, new Book("B006", "Operating System Concepts", "Abraham Silberschatz"), 3);
        librarian.addBook(library, new Book("B007", "Computer Networks", "Andrew S. Tanenbaum"), 5);
        librarian.addBook(library, new Book("B008", "Artificial Intelligence: A Modern Approach", "Stuart Russell"), 2);
        librarian.addBook(library, new Book("B009", "The Pragmatic Programmer", "Andy Hunt"), 4);
        librarian.addBook(library, new Book("B010", "Introduction to Algorithms", "Thomas H. Cormen"), 3);

        librarian.registerStudent(library, new Student("S001", "Aarav Sharma"));
        librarian.registerStudent(library, new Student("S002", "Isha Verma"));
        librarian.registerStudent(library, new Student("S003", "Rohan Mehta"));
        librarian.registerStudent(library, new Student("S004", "Priya Iyer"));
        librarian.registerStudent(library, new Student("S005", "Vivaan Desai"));
        librarian.registerStudent(library, new Student("S006", "Kavya Reddy"));
        librarian.registerStudent(library, new Student("S007", "Aditya Nair"));
        librarian.registerStudent(library, new Student("S008", "Sneha Kapoor"));
        librarian.registerStudent(library, new Student("S009", "Aryan Malhotra"));
        librarian.registerStudent(library, new Student("S010", "Diya Joshi"));

        System.out.println("Welcome to the Library System!");
        while (!exit) {
            System.out.println("\nSelect Role:");
            System.out.println("1. Librarian");
            System.out.println("2. Student");
            System.out.println("3. Exit");
            System.out.print("Choice: ");
            int choice = sc.nextInt();
            sc.nextLine(); // consume newline

            switch (choice) {
                case 1:
                    librarianMenu(library, sc);
                    break;
                case 2:
                    studentMenu(library, sc);
                    break;
                case 3:
                    exit = true;
                    System.out.println("Exiting... Goodbye!");
                    break;
                default:
                    System.out.println("Invalid option.");
            }
        }

    }

    private static void librarianMenu(Library library, Scanner sc) {
        boolean back = false;

        while (!back) {
            Librarian librarian = new Librarian("L001", "Head Librarian");
            System.out.println("\n-- Librarian Menu --");
            System.out.println("1. Add Book");
            System.out.println("2. Remove Book");
            System.out.println("3. Register Student");
            System.out.println("4. Remove Student");
            System.out.println("5. View All Books");
            System.out.println("6. Back");
            System.out.print("Choice: ");
            String choice = sc.nextLine();
            sc.nextLine();

            switch (choice) {
                case "1":
                    try {
                        System.out.print("Enter Book ID: ");
                        String bookId = sc.nextLine();
                        System.out.print("Enter Title: ");
                        String title = sc.nextLine();
                        System.out.print("Enter Author: ");
                        String author = sc.nextLine();
                        System.out.print("Enter Quantity: ");
                        int quantity = sc.nextInt();
                        sc.nextLine();

                        Book newBook = new Book(bookId, title, author);
                        librarian.addBook(library, newBook, quantity);
                    } catch (Exception e) {
                        System.out.println("Error adding book: " + e.getMessage());
                    }
                    System.out.print("Enter Book ID: ");
                    String bookId = sc.nextLine();
                    System.out.print("Enter Title: ");
                    String title = sc.nextLine();
                    System.out.print("Enter Author: ");
                    String author = sc.nextLine();
                    System.out.print("Enter Quantity: ");
                    int quantity = sc.nextInt();
                    sc.nextLine();

                    Book newBook = new Book(bookId, title, author);
                    librarian.addBook(library, newBook, quantity);
                    break;

                case "2":
                    System.out.print("Enter Book ID to remove: ");
                    String removeId = sc.nextLine();
                    librarian.removeBook(library, removeId);
                    break;

                case "3":
                    System.out.print("Enter Student ID: ");
                    String studentId = sc.nextLine();
                    System.out.print("Enter Student Name: ");
                    String studentName = sc.nextLine();
                    Student student = new Student(studentId, studentName);
                    librarian.registerStudent(library, student);
                    break;

                case "4":
                    System.out.print("Enter Student ID to remove: ");
                    String removeStudent = sc.nextLine();
                    librarian.removeStudent(library, removeStudent);
                    break;

                case "5":
                    System.out.println("All Books:");
                    for (Book book : library.getBooks().values()) {
                        System.out.println(book + " | Quantity: " + library.getBookQuantity(book.getBookId()));
                    }
                    break;

                case "6":
                    back = true;
                    break;

                default:
                    System.out.println("Invalid option.");
            }
        }
    }

    private static void studentMenu(Library library, Scanner sc) {
        System.out.print("Enter your Student ID: ");
        String studentId = sc.nextLine();
        Student student = library.getStudent(studentId);

        if (student == null) {
            System.out.println("Student not found.");
            return;
        }

        boolean back = false;
        while (!back) {
            System.out.println("\n-- Student Menu --");
            System.out.println("1. Search Book");
            System.out.println("2. Borrow Book");
            System.out.println("3. Return Book");
            System.out.println("4. Back");
            System.out.print("Choice: ");
            int choice = sc.nextInt();
            sc.nextLine();

            switch (choice) {
                case 1:
                    System.out.print("Enter keyword to search: ");
                    String keyword = sc.nextLine();
                    student.searchBook(library, keyword);
                    break;

                case 2:
                    System.out.print("Enter Book ID to borrow: ");
                    String borrowId = sc.nextLine();
                    student.borrowBook(library, borrowId);
                    break;

                case 3:
                    System.out.print("Enter Book ID to return: ");
                    String returnId = sc.nextLine();
                    student.returnBook(library, returnId);
                    break;

                case 4:
                    back = true;
                    break;

                default:
                    System.out.println("Invalid option.");
            }
        }
    }
}