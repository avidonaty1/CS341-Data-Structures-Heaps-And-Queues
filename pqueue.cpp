// CMSC 341 - Fall 2023 - Project 3
#include "pqueue.h"
PQueue::PQueue(prifn_t priFn, HEAPTYPE heapType, STRUCTURE structure) {
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
}
PQueue::~PQueue() {
    deleteSubTree(m_heap);
}

// recursively deletes each node in tree
void PQueue::deleteSubTree(Node* ptr) {
    if (ptr) {
        deleteSubTree(ptr->m_left);
        deleteSubTree(ptr->m_right);
        delete ptr;
    }
}

void PQueue::clear() {
    deleteSubTree(m_heap);
    m_heap = nullptr;
    m_size = 0;
}

PQueue::PQueue(const PQueue& rhs) {
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_size = rhs.m_size;
    m_heap = copyTree(rhs.m_heap);
}

// recursively goes through and adds left and right children
Node* PQueue::copyTree(const Node* ptr) {
    if (ptr == nullptr) {
        return  nullptr;
    }

    Node* temp = new Node(ptr->getPatient());
    temp->m_npl = ptr->m_npl;
    temp->m_left = copyTree(ptr->m_left);
    temp->m_right = copyTree(ptr->m_right);

    return temp;
}

PQueue& PQueue::operator=(const PQueue& rhs) {
    // protects from self-assignment
    if (this == &rhs) {
        return *this;
    }

    clear();

    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;

    if (rhs.m_heap != nullptr) {
        m_heap = copyTree(rhs.m_heap);
    }

    return *this;
}

void PQueue::mergeWithQueue(PQueue& rhs) {
    // protects from self-merging
    if (this == &rhs) {
        return;
    }

    // protects from merging with 2 different priority functions
    if (m_priorFunc != rhs.m_priorFunc || m_structure != rhs.m_structure) {
        throw domain_error("Queues have different structures or types");
    }

    // merges differently depending on structure
    if (m_structure == SKEW) {
        m_heap = mergeSkew(m_heap, rhs.m_heap);
    }
    else if (m_structure == LEFTIST) {
        m_heap = mergeLeftist(m_heap, rhs.m_heap);
        updateNPL(m_heap);
    }
    rhs.m_heap = nullptr;  
}

Node* PQueue::mergeSkew(Node* p1, Node* p2) {
    if (!p1) return p2;
    if (!p2) return p1;

    // swaps if needed
    if (m_heapType == MINHEAP) {
        if (m_priorFunc(p1->m_patient) > m_priorFunc(p2->m_patient)) {
            swap(p1, p2);
        }
    }
    else {
        if (m_priorFunc(p1->m_patient) < m_priorFunc(p2->m_patient)) {
            swap(p1, p2);
        }    
    }

    // adds it to right side, then swaps left and right children of p1 so it's on the left
    p1->m_right = mergeSkew(p1->m_right, p2);
    swap(p1->m_left, p1->m_right);

    return p1;
}

Node* PQueue::mergeLeftist(Node* p1, Node* p2) {
    if (!p1) return p2;
    if (!p2) return p1;

    // swaps if needed
    if (m_heapType == MINHEAP) {
        if (m_priorFunc(p1->m_patient) > m_priorFunc(p2->m_patient)) {
            swap(p1, p2);
        }
    }
    else {
        if (m_priorFunc(p1->m_patient) < m_priorFunc(p2->m_patient)) {
            swap(p1, p2);
        }    
    }

    // adds it to right side, then swaps left and right children of p1 if needed
    p1->m_right = mergeLeftist(p1->m_right, p2);

    if (!p1->m_left || (p1->m_left->m_npl < p1->m_right->m_npl)) {
        swap(p1->m_left, p1->m_right);
    }

    return p1;
}

// return minimum of 2 ints
int PQueue::min(int x, int y) {
    if (x < y) {
        return x;
    }
    else {
        return y;
    }
}

// return npl or -1 if nullptr
int PQueue::NPL(Node* ptr) {
    if (ptr == nullptr) {
        return -1;
    }

    return ptr->m_npl;
}

// recursively updates npl for every node in tree
void PQueue::updateNPL(Node* ptr) {
    // makes sure no seg faults
    if (ptr == nullptr) {
        return;
    }

    // traverses to left and right of tree
    updateNPL(ptr->m_left);
    updateNPL(ptr->m_right);

    int leftNPL = NPL(ptr->m_left);
    int rightNPL = NPL(ptr->m_right);

    ptr->m_npl = min(leftNPL, rightNPL) + 1;
}


void PQueue::insertPatient(const Patient& patient) {
    // creates the node to be inserted and adds it as root of new PQueue
    Node* newNode = new Node(patient);
    PQueue temp(m_priorFunc, m_heapType, m_structure);
    temp.m_heap = newNode;

    // merges our queue with new one
    mergeWithQueue(temp);
    m_size++;
}

// count is passed in by reference so it goes up for every node
int PQueue::numPatients() const {
    int count = 0;
    countPatients(m_heap, count);
    return count;
}

// iterates through tree and increases count for each node
void PQueue::countPatients(Node* ptr, int& count) const {
    if (ptr != nullptr) {
        count++;
        countPatients(ptr->m_left, count);
        countPatients(ptr->m_right, count);
    }
}

prifn_t PQueue::getPriorityFn() const {
    return m_priorFunc;
}

Patient PQueue::getNextPatient() {
    if (m_size == 0) {
        throw out_of_range("The heap is empty");
    }

    // stores original root to return the patient later
    Patient temp = m_heap->m_patient;

    // removes root node
    m_heap = removeRoot(m_heap);
    m_size--;

    return temp;
}

Node* PQueue::removeRoot(Node* ptr) {
    if (ptr == nullptr) {
        return nullptr;
    }

    // creates the new root to be merged
    Node* newRoot;

    // merges differently depending on structure
    if (m_structure == SKEW) {
        newRoot = mergeSkew(ptr->m_left, ptr->m_right);
        m_heap = newRoot;
    }
    else if (m_structure == LEFTIST) {
        newRoot = mergeLeftist(ptr->m_left, ptr->m_right);
        m_heap = newRoot;
        updateNPL(newRoot);
    }

    // deletes original root
    delete ptr;

    return newRoot;
}

void PQueue::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    if (m_heapType == heapType && m_priorFunc == priFn) return;

    // creates a new queue with different heaptype and inserts all the nodes
    PQueue temp(priFn, heapType, m_structure);
    temp.insertWithDiffStructure(m_heap, temp);

    *this = temp;
}

void PQueue::insertWithDiffStructure(Node* ptr, PQueue& temp) {
    if (ptr != nullptr) {
        insertWithDiffStructure(ptr->m_left, temp);
        temp.insertPatient(ptr->m_patient);
        insertWithDiffStructure(ptr->m_right, temp);
    }
}

void PQueue::setStructure(STRUCTURE structure){
    if (m_structure == structure) return;

    // creates a new queue with different structure and inserts all the nodes
    PQueue temp(m_priorFunc, m_heapType, structure);
    temp.insertWithDiffStructure(m_heap, temp);

    *this = temp;
}


STRUCTURE PQueue::getStructure() const {
    return m_structure;
}

HEAPTYPE PQueue::getHeapType() const {
    return m_heapType;
}

void PQueue::printPatientQueue() const {
    preOrder(m_heap);
}

void PQueue::preOrder(Node* node) const {
    if (node != nullptr) {
        cout << "[" << m_priorFunc(node->m_patient) << "] " << node->m_patient << endl;
        preOrder(node->m_left);
        preOrder(node->m_right);
    }
}

void PQueue::dump() const {
  if (m_size == 0) {
    cout << "Empty heap.\n" ;
  } else {
    dump(m_heap);
  }
  cout << endl;
}
void PQueue::dump(Node *pos) const {
  if ( pos != nullptr ) {
    cout << "(";
    dump(pos->m_left);
    if (m_structure == SKEW)
        cout << m_priorFunc(pos->m_patient) << ":" << pos->m_patient.getPatient();
    else
        cout << m_priorFunc(pos->m_patient) << ":" << pos->m_patient.getPatient() << ":" << pos->m_npl;
    dump(pos->m_right);
    cout << ")";
  }
}

ostream& operator<<(ostream& sout, const Patient& patient) {
  sout  << patient.getPatient() 
        << ", temperature: " << patient.getTemperature()
        << ", oxygen: " << patient.getOxygen()
        << ", respiratory: " << patient.getRR()
        << ", blood pressure: " << patient.getBP()
        << ", nurse opinion: " << patient.getOpinion();
  return sout;
}
ostream& operator<<(ostream& sout, const Node& node) {
  sout << node.getPatient();
  return sout;
}

// from here down are functions to help with testing

bool PQueue::heapPropertyMinTest() {
    return heapPropertyMin(m_heap);
}

bool PQueue::heapPropertyMin(Node* ptr) {
    if (ptr == nullptr) {
        return true;
    }

    if (ptr->m_left && m_priorFunc(ptr->m_patient) > m_priorFunc(ptr->m_left->m_patient)) {
        return false;
    }
    if (ptr->m_right && m_priorFunc(ptr->m_patient) > m_priorFunc(ptr->m_right->m_patient)) {
        return false;
    }

    return heapPropertyMin(ptr->m_left) && heapPropertyMin(ptr->m_right);
}

bool PQueue::heapPropertyMaxTest() {
    return heapPropertyMax(m_heap);
}

bool PQueue::heapPropertyMax(Node* ptr) {
    if (ptr == nullptr) {
        return true;
    }

    if (ptr->m_left && m_priorFunc(ptr->m_patient) < m_priorFunc(ptr->m_left->m_patient)) {
        return false;
    }
    if (ptr->m_right && m_priorFunc(ptr->m_patient) < m_priorFunc(ptr->m_right->m_patient)) {
        return false;
    }

    return heapPropertyMax(ptr->m_left) && heapPropertyMax(ptr->m_right);
}

bool PQueue::leftistProperty(Node* ptr) {
    if (ptr == nullptr) {
        return true;
    }

    if (NPL(ptr->m_left) < NPL(ptr->m_right)) {
        return false;
    }

    return leftistProperty(ptr->m_left) && leftistProperty(ptr->m_right);
}

bool PQueue::testNPL(Node* ptr) {
    if (ptr == nullptr) {
        return true;
    }

    int leftNPL = NPL(ptr->m_left);
    int rightNPL = NPL(ptr->m_right);

    if (ptr->m_npl != min(leftNPL, rightNPL) + 1) {
        return false;
    }

    return testNPL(ptr->m_left) && testNPL(ptr->m_right);
}