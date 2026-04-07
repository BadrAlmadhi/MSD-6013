#pragma once

////////////////////////////////////////////////////////////////////////
//
// Author: Badr Almadhi
// Date: April 4, 2026
//
// CS 6013
//
// Outline for SerialQueue class.  Fill in the missing data, comments, etc.
//
////////////////////////////////////////////////////////////////////////

template <typename T>
class SerialQueue {

public:
   SerialQueue() :
      head_( new Node{ T{}, nullptr } ), size_( 0 )
   {
      tail_ = head_;
   }

   void enqueue( const T & x ) {
      // Your code goes here.
      Node* newNode = new Node{ x, nullptr }; // create node

      tail_->next = newNode; // attach to end
      tail_ = newNode; // now becomes last node

      size_++; // track size
   }

   bool dequeue( T * ret ) {
      // Your code goes here.

      if (head_->next == nullptr) {
         return false;
      }

      Node* target = head_->next; //node to remove
      *ret = target->data; // copy value

      head_->next = target->next; // skip over node
     
      if (target == tail_) { // if last node
         tail_ = head_;
      }

      delete target; // free memeory 
      size_--; // update

      return true;
   }

   ~SerialQueue() {

      while( head_ != nullptr ) {
         Node* temp = head_->next;
         delete head_;
         head_ = temp;
      }
   }

   int size() const { return size_; }

private:

   struct Node {
      T      data;
      Node * next;
   };

   Node * head_;
   Node * tail_;
   int    size_;
};
