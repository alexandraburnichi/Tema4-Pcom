# Tema4-Pcom
Tema 4 Protocoale de Comunicatii - Client HTTP
Client Web. Comunicaţie cu REST API.

Pentru aceasta tema, m-am folosit de scheletul laboratorului 9, modificand in principal fisierul "requests.c", prin scoaterea argumentelor pe care nu le-am folosit in functiile compute_get_request si 
compute_post_request. De asemenea, in fisierul "requests.c" am mai adaugat o functie numita compute_delete_request.

Codul principal se afla in "client.c" si conține o funcție principală main care gestionează interacțiunea cu utilizatorul și trimite cererile corespunzătoare la serverul API. (fiecare tip de cerere --LOGIN, REGISTER, GET_BOOKS, etc-- este identificata prin getRequestType). 
La începutul programului, se realizează o conexiune la serverul API utilizând funcția open_connection. Apoi, programul intră într-un ciclu infinit care așteaptă comenzile utilizatorului și trimite cererile 
corespunzătoare către server. Putem iesi din acest ciclu cu ajutorul comenzii "exit".
De asemenea, am mai introdus niste functii auxiliare inaintea functiei main, care servesc la urmatoarele scopuri:

*create_json_object_book*
Această funcție primește detalii despre o carte (titlu, autor, număr de pagini, gen, editor) și creează un obiect JSON care conține aceste informații. Obiectul JSON rezultat este serializat într-un 
șir de caractere și returnat.

*extract_payload*
Această funcție primește un răspuns de la serverul API și extrage conținutul util (payload-ul) din răspuns. Funcția caută șirul "timeout" în răspuns pentru a identifica începutul conținutului util.Apoi, folosește funcția strtok pentru a extrage linii individuale din conținutul util și returnează rezultatul.

*verify_number_without_last și verify_number*
Aceste funcții verifică dacă un șir de caractere reprezintă un număr. verify_number_without_last verifică toate caracterele din șir, cu excepția ultimului caracter, în timp ce verify_number verifică toate caracterele din șir. Aceste funcții utilizează funcția isdigit din biblioteca ctype.h pentru a verifica dacă fiecare caracter este un cifră. Rezultatul este returnat sub formă de valoare booleană (1 pentru adevărat și 0 pentru fals).

Funcționalitatea principală a codului este implementată în funcția getRequestType care determină tipul comenzii primite de la utilizator și funcțiile asociate fiecărui tip de comandă. Tipurile de comenzi suportate sunt: REGISTER, LOGIN, ENTER_LIBRARY, GET_BOOKS, GET_BOOK, ADD_BOOK, DELETE_BOOK, LOGOUT, EXIT.Fiecare comandă are o secțiune în cod care primește argumentele necesare de la utilizator și apoi construiește cererea HTTP corespunzătoare folosind funcțiile din biblioteca helpers.h și requests.h.
Cererea este apoi trimisă la server și răspunsul este primit și procesat în consecință.

Am ales sa scriu tema in C, asa ca am folosit biblioteca parson. M-a ajutat faptul ca este o biblioteca simpla, lizibila si lightweight. Am folosit-o pentru a parsa detaliile despre carti si user (folosite la ADD_BOOK si LOGIN respectiv REGISTER).
