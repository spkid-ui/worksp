package IOOM_LAB.ASSIGNMENT6_3;

public class Student extends Member {
    public Student(String memberId, String name) {
        super(memberId, name);
    }

    public void searchBook(Library library, String keyword) {
        keyword = keyword.toLowerCase();
        boolean found = false;
        for (Book book : library.getBooks().values()) {
            if (book.getTitle().toLowerCase().contains(keyword) || book.getAuthor().toLowerCase().contains(keyword)) {
                System.out.println("Found: " + book);
                found = true;
            }
        }
        if (!found) {
            System.out.println("No books found with the keyword: " + keyword);
        }
    }

    public void borrowBook(Library library, String bookId) {
        try{
        library.lendBook(bookId, memberId);}
        catch (Exception e){
            System.out.println("Book not available for lending: " + e.getMessage());
        }
    }

    public void returnBook(Library library, String bookId) {
        try{
        library.returnBook(bookId, memberId);}
        catch(Exception e){
            System.out.println("Book not found in the library: " + e.getMessage());
        }
    }

    
}
