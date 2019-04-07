# Esenin C++

C++ wrapper for json requests to [esenin-server](https://github.com/esenin-org/esenin-server). 

### Dependencies

The library depends only on `libcurl`. 

### Installation

Clone this repo and run:

```bash
mkdir build && cd build
cmake ..
make && make install
```

This will install `libesenin.so` library on your computer.

### Usage

```cpp
#include "esenin.h"
#include <iostream>

int main (int argc, char *argv[])
{
  Client client = Client(std::string("127.0.0.1"), 9000L);
  std::vector<PosWord> words = client.get_pos("Мама мыла раму.");
  for(auto const& w: words) {
    std::cout << w.word << " " << w.pos << std::endl;
  }

  std::vector<std::string> tmp { "Мама", "мыла", "раму" };

  std::string id = client.fit_topics(
    std::vector<std::vector<std::string>> 
      {{"Мама", "мыла", "раму"}, {"Мама", "мыла", "окно"}, {"Мама", "мыла", "пол"}}, 10);

  for(auto const& t: client.get_topics(id, "Мама")) { 
    std::cout << t << ' ';
  }

  return 0;
}
```

### NLP Functions

`esenin.h` provides `Client` class which contains the following functions.

##### `Client(std::string const& ip, long port)`
Constructor to create `Client` instance that connects 
to the [esenin-server](https://github.com/esenin-org/esenin-server).

##### `std::vector<PosWord> get_pos(std::string const& text)`

Takes arbitrary _russian_ text and returns Part Of Speech tags. 

See [esenin-server](https://github.com/esenin-org/esenin-server#pos) for example of request and response. 


    std::vector<double> get_topics(std::string const& id, std::string const& term);

##### `std::string fit_topics(std::vector<std::vector<std::string>> const& terms, int topics)`
Takes list of documents, where document is a list of terms, and number of topics. 
Trains topic modeling algorithm with given terms and number of topics.
 
Returns the id of trained model, it's used in `get_topics` function.

See [esenin-server](https://github.com/esenin-org/esenin-server#tm) for example of request and response.

##### `std::vector<double> get_topics(std::string const& id, std::string const& term);`
Takes id of trained topic model and a term. 
Returns probabilities of a term to be in each topic. 

See [esenin-server](https://github.com/esenin-org/esenin-server#tm) for example of request and response.



