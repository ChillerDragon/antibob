# antibob

exploring ddnet's antibot interface

## compile

By default it expects the ddnet source code in the same
directory as antibot

```
git clone --recursive git@github.com:ddnet/ddnet
git clone git@github.com:ChillerDragon/antibob
cd antibob
make
```

But you can also provide the path to your server source as argument to make

```
git clone --recursive git@github.com:ddnet-insta/ddnet-insta /tmp/ddnet-insta
git clone git@github.com:ChillerDragon/antibob
cd antibob
make DDNET_DIR=/tmp/ddnet-insta
```

You might have to create a build/ directory in your DDNET_DIR
and build the server to get the generated network code which will
be accessible to antibob

## run

The make in the antibob repo will create a ``libantibot.so``
which has to be placed next to the ``DDNet-Server`` executable
when launching it. Then it will pick it up.
Also make sure to compile the server with the cmake flag
``-DANTIBOT=ON``

For development I like to symlink the libantibot.so into the servers build
directory. But be careful it will be overwritten once the server is rebuild.

```
cd antibob
make
cd ../ddnet/build
make
ln ../../antibob/libantibot.so .
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

#include "antibob.h"

class CMyAntibot : public CAntibob
{
public:
	void OnInit(CAntibotData *pData) override;
};
```

```C++
#include "my_antibot.h"

void CMyAntibot::OnInit(CAntibotData *pData)
{
	CAntibob::OnInit(pData);
	Log("my antibot initialized");
}
```

Then patch your class into interface.cpp


```C++
// interface.cpp

#include "my_antibot.h"

// [..]
extern "C" {

#define ANTIBOT_CLASS CMyAntibot // <= YOUR CLASS HERE

ANTIBOT_CLASS *pAntibob = nullptr;
```

