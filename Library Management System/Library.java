package IOOM_LAB.ASSIGNMENT6_3;
import java.util.*;
public class Library {
    private Map<String, Book> books = new HashMap<>(); // Map to store books with their ISBN as the key
    private Map<String, Integer> bookQuantities = new HashMap<>(); // Map to store book quantities with their ISBN as the key
    private Map<String, List<String>> borrowedBooks = new HashMap<>(); // Map to store borrowed books with user ID as the key
    private Map<String, Student> students = new HashMap<>(); // Map to store students with their ID as the key

    public void addBook(Book book, int quantity) {
        books.put(book.getBookId(), book);
        bookQuantities.put(book.getBookId(), quantity);
        System.out.println("Book added: " + book.getTitle() + " (Quantity: " + quantity + ")");
    }

    public void removeBook(Library library, String bookId) {
        if (books.containsKey(bookId)) {
            books.remove(bookId);
            bookQuantities.remove(bookId);
            System.out.println("Book removed: " + bookId);
        } else {
            System.out.println("Book not found: " + bookId);
        }
    }

    public void lendBook(String bookId, String userId) throws Exception{
        if(!books.containsKey(bookId)){
            throw new Exception("Book not found");
        }
        if(!students.containsKey(userId)){
            throw new Exception("User not found");
        }
        int quantity = bookQuantities.getOrDefault(bookId, 0);
        if(quantity<=0){
            throw new Exception("Book not available");
        }

        bookQuantities.put(bookId,quantity-1);
        borrowedBooks.computeIfAbsent(userId, k -> new ArrayList<>()).add(bookId);
        System.out.println("Book lent: " + bookId + " to user " + userId);
    }

    public void returnBook(String bookId, String userId) {
        if (borrowedBooks.containsKey(userId) && borrowedBooks.get(userId).remove(bookId)) {
            bookQuantities.put(bookId, bookQuantities.get(bookId) + 1);
            System.out.println("Book returned: " + books.get(bookId).getTitle() + " from " + userId);
        } else {
            System.out.println("Book not borrowed by user: " + bookId);
        }
    }
    public void registerMember(Student student) {
        students.put(student.getMemberId(), student);
        System.out.println("Student registered: " + student.getName());
    }
    public void removeMember(String memberId) {
        if (students.containsKey(memberId)) {
            students.remove(memberId);
            System.out.println("Student removed: " + memberId);
        } else {
            System.out.println("Student not found: " + memberId);
        }
    }

    public Map<String, Book> getBooks() {
        return books;
    }
    public Map<String, Integer> getBookQuantities() {
        return bookQuantities;
    }
    public int getBookQuantity(String bookId) {
        return bookQuantities.getOrDefault(bookId, 0);
    }
    public Map<String, Student> getStudents() {
        return students;
    }
    public Student getStudent(String memberId) {
        return students.get(memberId);
    }

}