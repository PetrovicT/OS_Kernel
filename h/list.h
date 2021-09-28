#ifndef LIST_H
#define LIST_H

#include "define.h"
#include <iostream.h>

template<class T> class List {
public:
	struct Node {
        T data;
        Node *prev;
        Node *next;
        Node(T _data) : data(_data), prev(nullptr), next(nullptr) {}
    };

    List() {
      	this->head = nullptr;
      	this->tail = nullptr;
      	this->numOfElem = 0;
    }

    virtual ~List() {
		while (head != nullptr) {
			Node* _delete = head;
			head = head->next;
			delete _delete;
		}
		numOfElem = 0;
		head = nullptr;
		tail = nullptr;
    }

    int numOfElem;
    Node *head;
    Node *tail;

    void printList() {
    	for (Node *p = head; p!=nullptr; p=p->next)	cout << p->data << " /element " << endl;
    }

	bool isEmpty() const {
		if (numOfElem == 0) return true;
		else return false;
	}

	int getSize() const { return numOfElem; }
	T getFirst() const {  return head->data;  }
	T getLast() const {  return tail->data;  }

    class Iterator;
    Iterator createIterHead() { return Iterator(this, head); }
    Iterator end() { return Iterator(this, nullptr); }

    void deleteFront() {
    	numOfElem--;
		Node* _delete = head;
		head = head->next;
		if (numOfElem == 0) head = tail = nullptr;
		else head->prev = nullptr;
		delete _delete;
    }

    void deleteBack() {
    	numOfElem--;
		Node* _delete = tail;
		tail = tail->prev;
		if (numOfElem == 0) head = tail = nullptr;
		else tail->next = nullptr;
		delete _delete;
    }

    void insertFront(T data) {
     	 Node* newNode = new Node(data);
         if (numOfElem == 0) {  tail = newNode;   }
         else { newNode->next = head;  head->prev = newNode; }
         head = newNode;
         numOfElem++;
    }

    void insertBack(T data) {
		Node* newNode = new Node(data);
		if (numOfElem == 0) { head = newNode; }
		else { newNode->prev = tail;  tail->next = newNode;	}
		tail = newNode;
		numOfElem++;
    }

    class Iterator {
     public:
    	Node* curr;
    	Node* next;
    	Node* prev;
    	List<T> *myList;
    	int index;

    	Iterator() {
    		this->curr = nullptr;
    		this->next = nullptr;
    		this->prev = nullptr;
    		this->myList = nullptr;
    		this->index = 0;
    	}

		Iterator(List<T>* myList, Node* curr){
			this-> curr = curr;
			this-> myList = myList;
			if (curr) next = curr->next;
			else next = nullptr;
			this->prev = nullptr;
			this->index = 0;
		}

		bool operator!=(const Iterator iter) { return curr != iter.curr;}
		T& operator*() {  return curr->data;  }

		Iterator& operator++() { 
			curr = next;
			if (curr) next = curr->next;
			else next = nullptr;
			return *this;
			
		}

		void insertBeforeArgument(T data) {
			if (myList->isEmpty() || curr->prev == nullptr) myList->insertFront(data);
			else if (curr == nullptr) myList->insertBack(data);
			else {
				List<T>::Node *newNode = new List<T>::Node(data);
				newNode->prev = curr->prev; newNode->next = curr;
				curr->prev->next = newNode;	curr->prev = newNode;
				myList->numOfElem++;
			}
		}

		void deleteCurrentNode(){
			if(myList->head == nullptr) return;
			if(curr == nullptr) return;
			if(myList->head == curr){
				myList->head = myList->head->next;
			}
			if(myList->tail == curr){
				myList->tail = myList->tail->prev;
			}
			if(curr->next != nullptr){
				curr->next->prev = curr->prev;
			}
			if(curr->prev != nullptr){
				curr->prev->next = curr->next;
			}
			delete curr;
			myList->numOfElem--;
			curr = nullptr;
		}
    };
};

#endif
