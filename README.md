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

