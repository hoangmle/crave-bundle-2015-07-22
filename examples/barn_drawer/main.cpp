#include <crave/ConstrainedRandom.hpp>
#include <crave/experimental/Experimental.hpp>

using namespace crave;

template<int V, int E> 
struct drawer : public crv_sequence_item {
  bool m_edges[V][V];
  crv_variable<unsigned> r_vertex[E + 1];
  crv_variable<unsigned> r_edges[V][V];
  
  crv_constraint c_edges { "c_edges" };
  
  drawer(crv_object_name) {
    for (int i = 0; i <= E; i++)
      c_edges(r_vertex[i]() < V);
    for (int i = 1; i <= E; i++)
      for (int v1 = 0; v1 < V; v1++)
        for (int v2 = 0; v2 < V; v2++)
          c_edges(
            if_then(
              r_vertex[i]() == v2 && r_vertex[i - 1]() == v1,
              (((r_edges[v1][v2]() >> i) & 2) == 2) && (reference(m_edges[v1][v2]) | reference(m_edges[v2][v1]))
            )
          );
    for (int v1 = 0; v1 < V; v1++)
      for (int v2 = 0; v2 < V; v2++)
          c_edges( 
            if_then(
              reference(m_edges[v1][v2]),
              bitcount(r_edges[v1][v2]()) + bitcount(r_edges[v2][v1]()) == 1
            )  
          );
  }

  void add_edge(int v1, int v2) {
    assert(v1 < V && v2 < V);   
    m_edges[v1][v2] = true;
    m_edges[v2][v1] = true;
    static unsigned e_cnt = 0;
    e_cnt++;
    assert(e_cnt <= E);    
  }

  void print() {
    for (int i = 0; i <= E; i++)
      std::cout << r_vertex[i] << " ";
    std::cout << std::endl;  
  }
  
};

int main (int argc , char *argv[]) {
  crave::init("crave.cfg");
  drawer<5, 8> d("drawer");
  d.add_edge(0, 1);
  d.add_edge(0, 2);
  d.add_edge(1, 2);
  d.add_edge(1, 3);
  d.add_edge(1, 4);
  d.add_edge(2, 3);
  d.add_edge(2, 4);
  d.add_edge(3, 4);
  assert(d.randomize());
  d.print();
	return 0;
}

