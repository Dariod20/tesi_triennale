variables
    lista: [int];
    elemento,target: int;

functions
    trova(lista: [int], elemento: int) -> int
     if lista == [] then -1
     else if head(lista) == elemento 
        then elemento
        else trova(tail(lista), elemento)
        end
     end;

    qualeStringa(n:int)->string
	if n == -1 then "\nL'elemento cercato NON è presente nella lista"
	else "\nL'elemento cercato è all'interno della lista"
	end;

body
    show "Questo programma trova un elemento selezionato della lista\n";
    lista = [1,8,2,6,8,138,88,8,3,4,8,18,8,9,6,3,6,7];
    show lista;
    elemento = 7;
    show "\nL'elemento da trovare è ";
    show elemento;
    target = trova(lista, elemento);
    show qualeStringa(target);
    