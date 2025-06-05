package IOOM_LAB.ASSIGNMENT6_3;

public class Librarian extends Member{
    public Librarian(String memberId, String name) {
        super(memberId, name);
    }
    public void addBook(Library library, Book book, int quantity) {
        library.addBook(book, quantity);
    }
    public void removeBook(Library library, String id) {
        library.removeBook(library, id);
    }
    public void registerStudent(Library library, Student student) {
        library.registerMember(student);
    }
    public void removeStudent(Library library, String memberId) {
        library.removeMember(memberId);
    }

}
