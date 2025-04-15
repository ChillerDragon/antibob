# antibob

Your ddnet antibot module boilerplate! Example antibot module with helper methods.
This is not a library so you can just fork this repository and start editing the code
to build your own antibot module.

## features

- Makefile
- tests
- github CI
- OOP wrapper around antibot api
- console command system like in ddnet ``antibot cmdlist``
- helper methods for sending chat messages (including automatic 0.6/0.7 translation)
- fully self contained and batteries included (no ddnet source code needed)

You can explore the most interesting code at [src/antibot.cpp](https://github.com/ChillerDragon/antibob/blob/master/src/bob/antibob.cpp)
the rest are just helpers that you should not have to worry about until you run into their limits.
All sample use cases are shown in antibot.cpp already.

## compile

```
git clone git@github.com:ChillerDragon/antibob
cd antibob
make
```

## run

The make in the antibob repo will create a ``libantibot.so``
which has to be placed next to the ``DDNet-Server`` executable
when launching it. Then it will pick it up.
Also make sure to compile the server with the cmake flag
``-DANTIBOT=ON``

For development I like to symlink the libantibot.so into the servers build
directory. But be careful it will be overwritten once the server is rebuild.

```
# assumes that you have the ddnet/ repo next to the antibob/ repo
#
# sources/
# ├── antibob
# └── ddnet
#

# build antibob
cd antibob
make

# navigate to ddnet repo
cd ../ddnet

# build ddnet server with antibot on
mkdir -p build
cd build
cmake .. -DANTIBOT=ON
make

# delete null antibot built with the server
rm libantibot.so

# replace it with symlink into antibob
# so recompiling antibob and restarting the ddnet server
# loads the new antibob automatically
ln ../../antibob/libantibot.so .

# verify it prints antibob not null antibot
./DDNet-Server | grep antibot
```

## clangd compile_commands.json

If your editor depends on a compile_commands.json to work properly.
You can use bear to generate them.

```
bear -- make
```

## Example usage

Create your own class that inherits from antibob.

```C++
// src/my_antibot.h

#include <bob/antibob.h>

class CMyAntibot : public CAntibob
{
public:
	using CAntibob::CAntibob;
	void OnInit(CAntibotData *pData) override;
};
```

```C++
#include "my_antibot.h"

void CMyAntibot::OnInit(CAntibotData *pData)
{
	CAntibob::OnInit(pData);
	log_info("antibot", "my antibot initialized");
}
```

Then patch your class into interface.cpp


```C++
// src/interface.cpp

#include "my_antibot.h"

// [..]
extern "C" {

#define ANTIBOT_CLASS CMyAntibot // <= YOUR CLASS HERE

ANTIBOT_CLASS *pAntibob = nullptr;
```

