import java.util.NoSuchElementException;
import java.util.Iterator;

public class Deque<Item> implements Iterable<Item> {
    private Node<Item> first;
    private Node<Item> last;
//    private  int size;

    public Deque() {
        first = null;
        last = null;
//        size = 0;
    }                           // construct an empty deque

    public boolean isEmpty() {
//        return size == 0;
        return first == last && last == null;
    }              // is the deque empty?

    public int size() {
//        return size;
        if (isEmpty()) {
            return 0;
        }

        int count = 0;
        for (Item i:this) {
            count++;
        }

        return count;
    }                // return the number of items on the deque


    public void addFirst(Item item) {
        nullCheck(item);
        if (first == null && last == null) {
            final Node<Item> itemNode = new Node<Item>(item, null, null);
            first =  itemNode;
            last = itemNode;
            return;
        }

        final Node<Item> oldFirst = first;
        first = new Node<Item>(item, oldFirst, null);
        if (oldFirst != null) {
            oldFirst.setPrevious(first);
        }
    }        // add the item to the front

    public void addLast(Item item)  {
        nullCheck(item);
        if (first == null && last == null) {
            final Node<Item> itemNode = new Node<Item>(item, null, null);
            first = itemNode;
            last = itemNode;
            return;
        }

        final Node<Item> newlast = new Node<Item>(item, null, last);
        last.setNext(newlast);
        last = newlast;
    }        // add the item to the end

    public Item removeFirst() {
        checkAvailability();
        final Node<Item> f = first;
        first = f.getNext();
        if (first == null) {
            last = null;
        } else {
            first.setPrevious(null);
        }

        return f.getObj();
    }         // remove and return the item from the front

    private void checkAvailability() {
        if (first == null && last == null) {
            throw new NoSuchElementException();
        }
    }

    public Item removeLast() {
        checkAvailability();
        final Node<Item> l = last;
        last = l.getPrevious();
        if (last != null) {
            last.setNext(null);
        } else {
            first = null;
            last = null;
        }

        l.setNext(null);
        l.setPrevious(null);

        return l.getObj();
    }            // remove and return the item from the end

    public Iterator<Item> iterator() {
        return new Iterator<Item>() {
            private Node<Item> currElement = first;
            @Override
            public boolean hasNext(){
                return currElement != null;
            }

            @Override
            public Item next() {
                if (currElement == null) {
                    throw new NoSuchElementException();
                }

                final Item obj = currElement.getObj();
                currElement = currElement.getNext();
                return obj;
            }

            @Override
            public void remove() {
                throw new UnsupportedOperationException();
            }
        };
    }        // return an iterator over items in order from front to end

    private  void nullCheck(Item item) {
//        if (item == null) throw new NullPointerException();
        if (item == null) {
            System.out.println("check null!");
        }
    }

    private static class Node<Item> {
        final private  Item obj;
        private  Node<Item> next;
        private  Node<Item> previous;

        public Node(Item obj, Node<Item> next, Node<Item> previous) {
            this.obj = obj;
            this.next = next;
            this.previous = previous;
        }

        public Item getObj() {
            return obj;
        }

        public Node<Item> getNext() {
            return next;
        }

        public void setNext(Node<Item> n) {
            this.next = n;
        }

        public void setPrevious(Node<Item> p) {
            this.previous = p;
        }

        public Node<Item> getPrevious() {
            return previous;
        }
    }
//    public static void main(String[] args) {

//    }   // unit testing (optional)
}