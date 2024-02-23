#include "pqueue.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

// Priority functions compute an integer priority for a patient.  Internal
// computations may be floating point, but must return an integer.

int priorityFn1(const Patient & patient);
int priorityFn2(const Patient & patient);

// a name database for testing purposes
const int NUMNAMES = 20;
string nameDB[NUMNAMES] = {
    "Ismail Carter", "Lorraine Peters", "Marco Shaffer", "Rebecca Moss",
    "Lachlan Solomon", "Grace Mclaughlin", "Tyrese Pruitt", "Aiza Green", 
    "Addie Greer", "Tatiana Buckley", "Tyler Dunn", "Aliyah Strong", 
    "Alastair Connolly", "Beatrix Acosta", "Camilla Mayo", "Fletcher Beck",
    "Erika Drake", "Libby Russo", "Liam Taylor", "Sofia Stewart"
};

// We can use the Random class to generate the test data randomly!
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }
    
    private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

int priorityFn1(const Patient & patient) {
    //this function works with a MAXHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [115-242]
    //temperature + respiratory + blood pressure
    //the highest priority would be 42+40+160 = 242
    //the lowest priority would be 35+10+70 = 115
    //the larger value means the higher priority
    int priority = patient.getTemperature() + patient.getRR() + patient.getBP();
    return priority;
}

int priorityFn2(const Patient & patient) {
    //this function works with a MINHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [71-111]
    //nurse opinion + oxygen
    //the highest priority would be 1+70 = 71
    //the lowest priority would be 10+101 = 111
    //the smaller value means the higher priority
    int priority = patient.getOpinion() + patient.getOxygen();
    return priority;
}

class Tester {
    public:

    // tests constructor
    bool constructorNormal() {
        bool result = true;

        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);

        result = result && (aQueue.m_heap == nullptr);
        result = result && (aQueue.m_size == 0);
        result = result && (aQueue.m_priorFunc == priorityFn2);
        result = result && (aQueue.m_heapType == MINHEAP);
        result = result && (aQueue.m_structure == LEFTIST);

        return result;
    }
    
    // tests copy constructor normal case
    bool copyConstructorNormal() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }
        
        PQueue bQueue(aQueue);

        bool result = true;
        // result = result && (aQueue.m_heap == bQueue.m_heap);
        result = result && (aQueue.m_heapType == bQueue.m_heapType);
        result = result && (aQueue.m_priorFunc == bQueue.m_priorFunc);
        result = result && (aQueue.m_structure == bQueue.m_structure);
        result = result && (aQueue.m_size == bQueue.m_size);

        return result;
    }

    // tests copy constructor into empty object
    bool copyConstructorEdge() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        PQueue bQueue(priorityFn1, MAXHEAP, SKEW);
        aQueue = bQueue;

        bool result = true;
        result = result && (aQueue.m_heapType == bQueue.m_heapType);
        result = result && (aQueue.m_size == bQueue.m_size);
        result = result && (aQueue.m_priorFunc == bQueue.m_priorFunc);
        result = result && (aQueue.m_structure == bQueue.m_structure);

        return result;
    }

    // tests overloaded assignment with empty object
    bool assignmentOperatorEdge() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        PQueue bQueue(priorityFn1, MAXHEAP, SKEW);
        bQueue = aQueue;

        bool result = true;
        // result = result && (aQueue.m_heap == bQueue.m_heap);
        result = result && (aQueue.m_heapType == bQueue.m_heapType);
        result = result && (aQueue.m_priorFunc == bQueue.m_priorFunc);
        result = result && (aQueue.m_structure == bQueue.m_structure);
        result = result && (aQueue.m_size == bQueue.m_size);

        return result;
    }

    // tests overaloaded assignment operator with normal objecg
    bool assignmentOperatorNormal() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        Random nameGen2(0,NUMNAMES-1);
        Random temperatureGen2(MINTEMP,MAXTEMP);
        Random oxygenGen2(MINOX,MAXOX);
        Random respiratoryGen2(MINRR,MAXRR);
        Random bloodPressureGen2(MINBP,MAXBP);
        Random nurseOpinionGen2(MINOPINION,MAXOPINION);
        PQueue bQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<4;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            bQueue.insertPatient(patient);
        }
        bQueue = aQueue;

        bool result = true;
        // result = result && (aQueue.m_heap == bQueue.m_heap);
        result = result && (aQueue.m_heapType == bQueue.m_heapType);
        result = result && (aQueue.m_priorFunc == bQueue.m_priorFunc);
        result = result && (aQueue.m_structure == bQueue.m_structure);
        result = result && (aQueue.m_size == bQueue.m_size);

        return result;
    }

    bool insertTest() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        return aQueue.m_size == 10;
    }

    bool removeTestNormal() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<10;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        aQueue.getNextPatient();

        return aQueue.m_size == 9;
    }

    bool removeTestError() {
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);

        try {
            aQueue.getNextPatient();
            return false;
        }
        catch(out_of_range& e) {
            return true;
        }

        return true;
    }

    // tests heap property for inserting min heap
    bool insertionMinHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<300;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        return aQueue.heapPropertyMinTest();
    }

    // tests heap property for inserting max heap
    bool insertionMaxHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn1, MAXHEAP, LEFTIST);
        for (int i=0;i<300;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        return aQueue.heapPropertyMaxTest();
    }

    // tests min heap removals
    bool removalMinHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<300;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        for (int i = 0; i < 100; i++) {
            aQueue.getNextPatient();
        }

        return aQueue.heapPropertyMinTest();
    }

    // tests max heap removals
    bool removalMaxHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn1, MAXHEAP, LEFTIST);
        for (int i=0;i<300;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        for (int i = 0; i < 100; i++) {
            aQueue.getNextPatient();
        }

        return aQueue.heapPropertyMaxTest();
    }

    // tests leftist property for min heap
    bool leftistPropertyMinHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<20;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        return aQueue.leftistProperty(aQueue.m_heap);
    }

    // tests leftist property for max heap
    bool leftistPropertyMaxHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn1, MAXHEAP, LEFTIST);
        for (int i=0;i<11;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        return aQueue.leftistProperty(aQueue.m_heap);
    }

    // tests npl values for min heap
    bool testNPLValueMinHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<20;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        return aQueue.testNPL(aQueue.m_heap);
    }

    // tests npl values for max heap
    bool testNPLValueMaxHeap() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn1, MAXHEAP, LEFTIST);
        for (int i=0;i<20;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        return aQueue.testNPL(aQueue.m_heap);
    }

    bool setPriority() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<20;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        int rootOriginal = aQueue.m_priorFunc(aQueue.m_heap->m_patient);
        aQueue.setPriorityFn(priorityFn1, MAXHEAP);
        int rootChanged = aQueue.m_priorFunc(aQueue.m_heap->m_patient);
        bool result = true;

        result = result && (aQueue.m_priorFunc == priorityFn1);
        result = result && (rootOriginal != rootChanged);
        result = result && (aQueue.m_heapType == MAXHEAP);

        return result;
    }

    // tests merge for normal case
    bool mergeNormal() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<1;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }

        Random nameGen2(0,NUMNAMES-1);
        Random temperatureGen2(MINTEMP,MAXTEMP);
        Random oxygenGen2(MINOX,MAXOX);
        Random respiratoryGen2(MINRR,MAXRR);
        Random bloodPressureGen2(MINBP,MAXBP);
        Random nurseOpinionGen2(MINOPINION,MAXOPINION);
        PQueue bQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<1;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            bQueue.insertPatient(patient);
        }

        aQueue.mergeWithQueue(bQueue);

        return (aQueue.m_heap->m_left || aQueue.m_heap->m_right);
    }

    // tests merge with empty queue
    bool mergeEdge() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        for (int i=0;i<1;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
        }
        
        PQueue bQueue(priorityFn2, MINHEAP, LEFTIST);
        aQueue.mergeWithQueue(bQueue);

        return (!aQueue.m_heap->m_left && !aQueue.m_heap->m_right);
    }

    // tests case to merge with different structure/priority function
    bool mergeError() {
        Random nameGen(0,NUMNAMES-1);
        Random temperatureGen(MINTEMP,MAXTEMP);
        Random oxygenGen(MINOX,MAXOX);
        Random respiratoryGen(MINRR,MAXRR);
        Random bloodPressureGen(MINBP,MAXBP);
        Random nurseOpinionGen(MINOPINION,MAXOPINION);
        PQueue aQueue(priorityFn2, MINHEAP, LEFTIST);
        PQueue bQueue(priorityFn1, MINHEAP, SKEW);
        for (int i=0;i<1;i++){
            Patient patient(nameDB[nameGen.getRandNum()],
                        temperatureGen.getRandNum(),
                        oxygenGen.getRandNum(),
                        respiratoryGen.getRandNum(),
                        bloodPressureGen.getRandNum(),
                        nurseOpinionGen.getRandNum());
            aQueue.insertPatient(patient);
            bQueue.insertPatient(patient);
        }

        try {
            aQueue.mergeWithQueue(bQueue);
            return false;
        }
        catch(domain_error& e) {
            return true;
        }

        return true;
    }

};


int main() {
    Tester test;

    cout << endl;

    // test constructors
    if (test.constructorNormal()) {
        cout << "Normal Constructor passed" << endl;
    }
    else {
        cout << "Normal Constructor failed" << endl;
    }

    if (test.copyConstructorNormal()) {
        cout << "Copy constructor normal case passed" << endl;
    }
    else {
        cout << "Copy constructor normal case failed" << endl;
    }

    if (test.copyConstructorEdge()) {
        cout << "Copy constructor edge case passed" << endl;
    }
    else {
        cout << "Copy constructor edge case failed" << endl;
    }
    cout << endl;

    // tests assignment operator cases
    if (test.assignmentOperatorNormal()) {
        cout << "Overloaded assignment operator normal case passed" << endl;
    }
    else {
        cout << "Overloaded assignment operator normal case failed" << endl;
    }

    if (test.assignmentOperatorEdge()) {
        cout << "Overloaded assignment operator edge case passed" << endl;
    }
    else {
        cout << "Overloaded assignment operator edge case failed" << endl;
    }
    cout << endl;

    // tests insert function
    if (test.insertTest()) {
        cout << "Insert function test passed" << endl;
    }
    else {
        cout << "Insert function test failed" << endl;
    }
    cout << endl;

    // tests getNextPatient function
    if (test.removeTestNormal()) {
        cout << "Get next patient normal test passed" << endl;
    }
    else {
        cout << "Get next patient normal test failed" << endl;
    }

    if (test.removeTestError()) {
        cout << "Get next patient error case passed" << endl;
    }
    else {
        cout << "Get next patient error case failed" << endl;
    }
    cout << endl;

    // tests insertion properties
    if (test.insertionMinHeap()) {
        cout << "Min heap insertion property test passed" << endl;
    }
    else {
        cout << "Min heap insertion property test failed" << endl;
    }

    if (test.insertionMaxHeap()) {
        cout << "Max heap insertion property test passed" << endl;
    }
    else {
        cout << "Max heap insertion property test failed" << endl;
    }
    cout << endl;

    // tests removal properties
    if (test.removalMinHeap()) {
        cout << "Min heap removal property test passed" << endl;
    }
    else {
        cout << "Min heap removal property test failed" << endl;
    }

    if (test.removalMaxHeap()) {
        cout << "Max heap removal property test passed" << endl;
    }
    else {
        cout << "Max heap removal property test failed" << endl;
    }
    cout << endl;

    // tests npl properties
    if (test.leftistPropertyMinHeap()) {
        cout << "Min heap leftist property passed" << endl;
    }
    else {
        cout << "Min heap leftist property failed" << endl;
    }

    if (test.leftistPropertyMaxHeap()) {
        cout << "Max heap NPL property passed" << endl;
    }
    else {
        cout << "Max heap NPL property failed" << endl;
    }
    cout << endl;

    // tests npl values
    if (test.testNPLValueMinHeap()) {
        cout << "Min heap npl values passed" << endl;
    }
    else {
        cout << "Min heap npl values failed" << endl;
    }

    if (test.testNPLValueMaxHeap()) {
        cout << "Max heap npl values passed" << endl;
    }
    else {
        cout << "Max heap npl values failed" << endl;
    }
    cout << endl;

    // tests setPriority function
    if (test.setPriority()) {
        cout << "Set Priority function test passed" << endl;
    }
    else {
        cout << "Set Priority function test failed" << endl;
    }
    cout << endl;

    // tests merge function
    if (test.mergeNormal()) {
        cout << "Merge normal case passed" << endl;
    }
    else {
        cout << "Merge normal case failed" << endl;
    }

    if (test.mergeEdge()) {
        cout << "Merge edge case passed" << endl;
    }
    else {
        cout << "Merge edge case failed" << endl;
    }

    if (test.mergeError()) {
        cout << "Merge error case passed" << endl;
    }
    else {
        cout << "Merge error case failed" << endl;
    }
    cout << endl;


    return 0;
}