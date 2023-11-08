#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>
//#define SENTINEL 1
void right_rotate(rbtree *t, node_t *cur);
void left_rotate(rbtree *t, node_t *cur);
void insert_fixup(rbtree *t, node_t *cur );
void postorder_delete(rbtree *t, node_t *cur);
node_t* erase_min(rbtree *t, node_t *x);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
void rbtree_erase_fixup(rbtree *t, node_t *x);
void inorder_to_array(const rbtree *t, key_t *arr, node_t *node, int* index);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  node_t *nilnode = (node_t *)calloc(1, sizeof(node_t));
  nilnode->color = RBTREE_BLACK;
  nilnode->parent = NULL;
  nilnode->left = NULL;
  nilnode->right = NULL;

  p->nil = nilnode;
  p->root = nilnode;
  
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  postorder_delete(t, t->root);
  t->root = t->nil;
  free(t->nil);
  free(t); 
}

void postorder_delete(rbtree *t, node_t *cur){
  t->root = t->nil;
  if(cur== t->nil){
    return;
  }
  postorder_delete(t, cur->left);
  postorder_delete(t, cur->right);
  free(cur);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *new = (node_t*)calloc(1, sizeof(node_t));
  new->key = key;
  new->color = RBTREE_RED;
  node_t *p_node = t->nil;
  node_t *cur = t->root;
  while (cur!=t->nil){
    p_node = cur;
    if (new->key < cur->key){
      cur = cur->left;      
    }
    else{
      cur = cur->right;
    }
  }

  new->parent = p_node;
  if(p_node == t->nil){
    t->root = new;
    new ->color = RBTREE_BLACK;
  }
  else if(new->key < p_node->key){
    p_node->left = new;
  }
  else{
    p_node->right = new;
  }
  new->left = t->nil;
  new->right = t->nil;
  
  insert_fixup(t, new);
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *cur = t->root;
  while(cur!=t->nil){
    if(cur->key > key){ cur= cur->left;}          // 현재 key값보다 key가 작으면 좌측 자식 노드 탐색.
    else if(cur->key < key){  cur= cur->right;}   // 현재 key값보다 key가 크면 우측 자식 노드 탐색.
    else{ return cur;}                                 // 현재 key값과 key가 같으면 탐색 성공.
  }
  // 노드가 존재하지 않으면 NULL 리턴.
  return NULL;
}
  
node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *temp = t->root;
  while(temp->left!=t->nil){
    temp=temp->left;
  }
  return temp;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *temp = t->root;
  while(temp->right!=t->nil){
    temp=temp->right;
  }
  return temp;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *y = p;
  node_t *x;
  color_t y_org_color = y->color;
  if(p->left==t->nil){
    x=p->right;
    rbtree_transplant(t, p, p->right);
  }
  else if(p->right==t->nil){
    x=p->left;
    rbtree_transplant(t, p, p->left);
  }
  else{
    y = erase_min(t, p->right);
    y_org_color = y->color;
    x=y->right;
    if (y->parent == p){
      x->parent = y;
    }
    else{
      rbtree_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    rbtree_transplant(t,p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  if (y_org_color==RBTREE_BLACK){
    rbtree_erase_fixup(t, x);
  }
  free(p);
  return 0;
}

node_t* erase_min(rbtree *t, node_t *x){
  node_t *temp= x;
  if(temp==t->nil){
    return temp;
  }
  while(temp->left!=t->nil){
    temp=temp->left; //!!!!
  }
  return temp;
}

void rbtree_erase_fixup(rbtree *t, node_t *x){
  node_t* w;
  while((x!=t->root) && (x->color == RBTREE_BLACK)){
    if(x==x->parent->left){
      w = x->parent->right;
      if(w->color ==RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{ 
        if(w->right->color ==RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w= x->parent-> right;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else{
      w = x->parent->left;
      if(w->color ==RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if(w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->left->color ==RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w= x->parent-> left;
        }
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent==t->nil){
    t->root = v;
  }
  else if(u==u->parent->left){
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int cnt=0;
  int* index = &cnt;
  inorder_to_array(t, arr, t->root, index);
  return 0;
}

void inorder_to_array(const rbtree *t, key_t *arr, node_t *node, int* index){
  if(node == t->nil){
    return;
  }
  inorder_to_array(t, arr, node->left, index);
  arr[*index] = node->key;
  *index +=1;
  inorder_to_array(t, arr, node->right, index);
  return;
}
void insert_fixup(rbtree *t, node_t *cur ){
  /*  좌트리  */
  node_t *uncle;
  while(cur->parent->color==RBTREE_RED){             // 현재 노드가 빨강인 동안 반복.
    // 1. Case1. 부모&삼촌 모두 RED
    if(cur->parent == cur->parent->parent->left){    // 부모노드가 할아버지 노드의 Left-Child이면
      uncle = cur->parent->parent->right;    // 삼촌 노드 지정
      if(uncle->color==RBTREE_RED){                  // 삼촌 노드가 빨강이면
        cur->parent->color = RBTREE_BLACK;           // 부모 노드 검정 변경.
        uncle->color = RBTREE_BLACK;                 // 삼촌 노드 검정 변경.
        cur->parent->parent->color = RBTREE_RED;     // 할아버지 노드 빨강 변경.
        cur = cur->parent->parent;                   // 현재 노드 할아버지 노드로 옮기고 할아버지 노드에서 탐색
      }
      else{              // 현재 노드가 부모노드의 오른쪽 자식이면
        if(cur==cur->parent->right){
          cur = cur->parent;                           // 현재 노드를 부모노드로 설정.
          left_rotate(t, cur);                         // 왼쪽으로 회전
        }                         
        cur->parent->color = RBTREE_BLACK;           // 현재 노드(원래 노드의 부모)의 부모(원래 노드의 할아버지 노드) 검정으로 변경.
        cur->parent->parent-> color = RBTREE_RED;    // 현재 노드(원래 노드의 부모)의 할아버지 노드 빨강으로 변경.
        right_rotate(t,cur->parent->parent);        // 우측 회전
      }
    }
    else{
      node_t *uncle = cur->parent->parent->left;
      if(uncle!= t->nil && uncle->color==RBTREE_RED){
        cur->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        cur->parent->parent->color = RBTREE_RED;
        cur = cur->parent->parent;
      }
      else{
        if( cur == cur -> parent ->left)
        {
          cur=cur->parent;
          right_rotate(t, cur);
        }

        if(cur != t->root && cur->parent != t->root){
          cur->parent->color = RBTREE_BLACK;
          cur -> parent->parent->color = RBTREE_RED;
          left_rotate(t, cur->parent->parent);
        }
      }
    }      
  }     
  t->root->color = RBTREE_BLACK;
}

void left_rotate(rbtree *t, node_t *cur){
  node_t* rC = cur ->right;
  cur->right = rC->left;
  if (rC->left != t->nil){
    rC->left->parent = cur;
  }
  rC->parent = cur->parent;
  if(cur->parent == t->nil){
    t->root = rC;
  }
  else if(cur==cur->parent->left){
    cur->parent->left = rC;
  }
  else{
    cur->parent->right = rC;
  }
  rC->left = cur;
  cur->parent = rC;
  return;
}

void right_rotate(rbtree *t, node_t *cur){
  node_t* lC = cur ->left;
  cur->left = lC->right;
  if (lC->right != t->nil){
    lC->right->parent=cur;
  }
  lC->parent = cur->parent;
  if(cur->parent == t->nil){
    t->root = lC;
  }
  else if(cur==cur->parent->right){
    cur->parent->right = lC;
  }
  else{
    cur->parent->left = lC;
  }
  lC->right = cur;
  cur->parent = lC;
}

// 디버깅용 중위순회 출력 함수.
void midorder_print(rbtree *t, node_t *cur){
  if(cur== t->nil){
    return;
  }
  midorder_print(t, cur->left);
  printf("key: %d, color: %d, left: %d, right : %d\n", cur->key, cur->color, cur->left->key, cur->right->key);
  midorder_print(t, cur->right);
}

// int main(void){
//   printf("Start code!\n");
//   rbtree *t = new_rbtree();
//   node_t *a = rbtree_insert(t, 10);
//   node_t *b = rbtree_insert(t, 5);
//   node_t *c = rbtree_insert(t, 8);
//   node_t *d = rbtree_insert(t, 34);
//   node_t *e = rbtree_insert(t, 67);
//   node_t *y = rbtree_insert(t, 23);
//   node_t *g = rbtree_insert(t, 156);
//   node_t *h = rbtree_insert(t, 24);
//   node_t *i = rbtree_insert(t, 2);
//   node_t *j = rbtree_insert(t, 12);
//   node_t *f = rbtree_insert(t, 36);
//   node_t *r = rbtree_insert(t, 990);
//   node_t *z = rbtree_insert(t, 25);

//   midorder_print(t, t->root);
//   //printf("insert : %p\n", a);
//   //printf("find : %p\n", z);
//   //printf("erase : %d", y);
// }