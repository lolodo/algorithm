/*
链表操作，
（1）.单链表就地逆置，
（2）合并链表
ANSWER
Reversing a linked list. Already done.

What do you mean by merge? Are the original lists sorted and need to be kept sorted? If not, are there any special requirements?
I will only do the sorted merging.

Node * merge(Node * h1, Node * h2) {
  if (h1 == NULL) return h2;
  if (h2 == NULL) return h1;
  Node * head;
  if (h1->data>h2->data) {
    head = h2; h2=h2->next;
  } else {
    head = h1; h1=h1->next;
  }
  Node * current = head;
  while (h1 != NULL && h2 != NULL) {
    if (h1 == NULL || (h2!=NULL && h1->data>h2->data)) {
      current->next = h2; h2=h2->next; current = current->next;
    } else {
      current->next = h1; h1=h1->next; current = current->next;
    } 
  }
  current->next = NULL;
  return head;
}
*/
