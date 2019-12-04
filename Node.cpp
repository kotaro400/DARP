#include"Node.hpp"

//**********************//
// Node					//
//**********************//
Node::Node(){
	//fixed
	this->index = 0;
	this->location = nullptr;
	//variable
	this->order = 0;
	this->pre = nullptr;
	this->next = nullptr;
	//Status
	this->time = 0.0;
}

void Node::setType(const NodeType type) {
	this->type = type;
}
void Node::setIndex(const int value) {
	this->index = value;
}
void Node::setLocation(const Location* location) {
	this->location = location;
}

void Node::setOrder(const int value) {
	this->order = value;
}
void Node::setPre(Node* pre) {
	this->pre = pre;
}
void Node::setNext(Node* next) {
	this->next = next;
}

// void Node::initStatus(const int r, const int p) {//have to modify
// 	this->cap.resize(r);
	
// }
// void Node::initStatus(const int r, const int p, const double b) {//have to modify
// 	this->cap.resize(r);
// }

void Node::setCap(const int cap) {
	this->cap = cap;
}
void Node::setTime(const double time) {
	this->time = time;
}

Node::NodeType Node::getType() const {
	return this->type;
}
int Node::getIndex() const {
	return this->index;
}
const Location* Node::getLocation() const {
	return this->location;
}

int Node::getOrder() const {
	return this->order;
}
Node* Node::getPre() const {
	return this->pre;
}
Node* Node::getNext() const {
	return this->next;
}

// const vector<int>* Node::getCap() const {
// 	return &this->cap;
// }
// double Node::getTime() const {
// 	return this->time;
// }


void Node::updateOrder() {
	this->order = this->getPre()->getOrder() + 1;
}

// **********************//
// Request				//
// **********************//
Request::Request() {
}

// void Request::setCustomer(const Customer* customer) {
// 	this->customer = customer;
// }
// const Customer* Request::getCustomer() const {
// 	return this->customer;
// }

//**********************//
// Pick					//
//**********************//
Pickup::Pickup() {
	this->drop = nullptr;
}

void Pickup::setDrop(Node* drop) {
	this->drop = drop;
}
Node* Pickup::getDrop() const {
	return this->drop;
}

// void Pickup::updateCap() {
// 	int temp_int;
// 	vector<int> result;
// 	const vector<int>* pre_cap, *demand;

// 	pre_cap = this->getPre()->getCap();
// 	demand = this->getCustomer()->getDemand();
// 	result.reserve(pre_cap->size());

// 	for (size_t i = 0;i < pre_cap->size();i++) {
// 		temp_int = (*pre_cap)[i];
// 		temp_int += (*demand)[i];
// 		result.push_back(temp_int);
// 	}
// 	this->setCap(result);
// }

//**********************//
// Drop					//
//**********************//
Dropoff::Dropoff() {
	this->pick = nullptr;
}

void Dropoff::setPick(Node* pick) {
	this->pick = pick;
}
Node* Dropoff::getPick() const {
	return this->pick;
}

// void Dropoff::updateCap() {
// 	int temp_int;
// 	vector<int> result;
// 	const vector<int>* pre_cap, *demand;

// 	pre_cap = this->getPre()->getCap();
// 	// demand = this->getCustomer()->getDemand();
// 	result.reserve(pre_cap->size());

// 	for (size_t i = 0;i < pre_cap->size();i++) {
// 		temp_int = (*pre_cap)[i];
// 		temp_int -= (*demand)[i];
// 		result.push_back(temp_int);
// 	}
// 	this->setCap(result);
// }


//**********************//
// Dpot					//
//**********************//
Depot::Depot() {
	this->servicetime = 0.0;
}

void Depot::setServicetime(const double value) {
	this->servicetime = value;
}
double Depot::getServicetime() const {
	return this->servicetime;
}