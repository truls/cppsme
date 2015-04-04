// SME


#include <iostream>
#include <initializer_list>
#include <vector>
#include <set>
#include <map>

#include "sme.h"

SyncProcess::SyncProcess(const std::string name,
			 Busses inputs,
			 Busses outputs)
  :name{name} {
  for(Bus* b : inputs){
    ins.push_back(b);
    if (b->is_named()) {
      ins_map.insert({b->get_name(), b});
    }
  }
  for(Bus* b : outputs){
    outs.push_back(b);
    if (b->is_named()) {
      outs_map.insert({(b->get_name()), b});
    }
  }
}

SyncProcess::~SyncProcess() {}

Bus* SyncProcess::get_in(int k) {
  return ins[k];
}
Bus* SyncProcess::get_in(Name n) {
  return ins_map[n];
}

Bus* SyncProcess::get_out(int k) {
  return outs[k];
}

Bus* SyncProcess::get_out(Name n) {
  return outs_map[n];
}

vector<Bus*> SyncProcess::get_ins() {
  return ins;
}

vector<Bus*> SyncProcess::get_outs() {
  return outs;
}

Run::Run(int steps)
  :steps{steps} {}

void Run::add_proc(std::initializer_list<SyncProcess*> list) {
  for (auto e:list) {
    add_proc(e);
  }
}

void Run::add_proc(SyncProcess* p) {
  procs.push_back(p);
  auto ins = p->get_ins();
  auto outs = p->get_outs();

  for (Bus* p: ins) {
    busses.insert(p);
  }
  for (Bus* p: outs) {
    busses.insert(p);
  }
}

void Run::start() {
  int i;
  for(i = 0; i < steps; i++){
    for (Bus* b: busses){
      b->step();
    }
    for (SyncProcess* e:this->procs) {
      e->step();
    }
  }
  for (SyncProcess* e: procs) {
    delete e;
  }
}

Bus::Bus() {}
Bus::Bus(Name name)
  :name{static_cast<Name>(name)}, named{true} {}

void Bus::step() {
  _out = _in;
}

int Bus::read(){
  return _out;
}

void Bus::write(int v) {
  _in = v;
Name Bus::get_name() {
   return name;
 }

bool Bus::is_named() {
   return named;
 }

void Bus::assign_to(Bus** b) {
  *b = this;
}

void Bus::assign(Busses from, BussesPtrPtr to) {
  if (from.size() != to.size()) {
    std::cout << "Cannot assign busses. to,from not equal length." << std::endl;
    // TODO: how to handle error?
  }
  auto fromit = from.begin();
  auto toit = to.begin();
  auto fromit_end = from.end();

  while (fromit != fromit_end) {
    (*fromit)->assign_to(*toit);
    fromit++;
    toit++;
  }
}

void Bus::assign(Busses from1, BussesPtrPtr to1, Busses from2, BussesPtrPtr to2) {
  Bus::assign(from1, to1);
  Bus::assign(from2, to2);
}

//
