variables
    current: int;

functions
    nextPrime(n: int) -> int
        if naivePrime(n) then
            n
        else
            nextPrime(n+1)
        end;

    divides(a: int, b: int) -> bool
        a/b*b == a;

    naivePrime(n: int) -> bool
        primeRecursive(n, 2);

    primeRecursive(n: int, i: int) -> bool
        if n <= 2 or divides(n, i) or i*i > n then
            if n == 2 then
                true
            else
                not divides(n, i)
            end
        else
            primeRecursive(n, i+1)
        end;

body
    show "Inserisci un numero:";
    current = ?(int);
    show "\nIl prossimo numero primo >= a quello inserito è:";
    show nextPrime(current);