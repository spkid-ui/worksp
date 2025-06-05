package IOOM_LAB.ASSIGNMENT6_3;

public abstract class Member {
    protected String memberId;
    protected String name;

    public Member(String memberId, String name) {
        this.memberId = memberId;
        this.name = name;
    }

    public String getMemberId() {
        return memberId;
    }
    public String getName() {
        return name;
    }
    
}
