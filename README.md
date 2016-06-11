Rele is an incomplete HTTP server which dynamically compiles and links routes
written in c++11. This allows you to write backend code in c++11 without the
need to manually triggering make.

Documentation is currently shoddy, but it'll be improved whenever.

Basic usage:

$ make
$ rele <port>
...

Access the server in your browser http://localhost:<port>, the only route that
is available is under http://localhost:<port>/myroute. Accessing myroute for
the first time should trigger the make process withing the server.

To generate the documentation, you can do:

$ make docs

For this you need to have installed http://naturaldocs.org/

---

Oh! Note that there are quite a few quirks, this is due to the fact that I've
ported this a couple of times over the years (c -> c++ -> c++11).
