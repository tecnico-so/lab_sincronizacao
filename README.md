# Guião sobre sincronização de secções críticas

![IST](img/IST_DEI.png)

## Objetivos

No final deste guião, deverá ser capaz de:

- identificar secções críticas em programas que utilizam tarefas (*threads*);
- interpretar os diagnósticos do *ThreadSanitizer*;
- corrigir problemas de sincronização de secções críticas usando trincos lógicos (*mutex*);
- distinguir situações em que um trinco de leitura-escrita (*read-write lock*) permite maior concorrência do que um *mutex*.

### Antes de começar

*i)* Para os exemplos e o exercício vai precisar de um sistema operativo compatível com POSIX, como o Ubuntu Linux ou outro.
Se ainda não o tiver disponível no seu computador pessoal, pode utilizar um dos computadores do laboratório.

*ii)* Revisite o [guião sobre deteção de erros](https://github.com/tecnico-so/lab_detecao-erros) onde são apresentados os sanitizadores de código.
Iremos utilizar o *ThreadSanitizer*.

*iii)* Para obter os exemplos de código, clone este repositório, usando o comando: ``git clone https://github.com/tecnico-so/lab_sincronizacao.git``


## 1. Tarefas 

O exemplo de código é uma aplicação bancária onde dois utilizadores, Alice e Bob, acedem a uma conta partilhada.
A Alice deposita dinheiro enquanto o Bob tenta levantá-lo.
Estas operações são executadas por tarefas concorrentes que acedem à mesma conta. 
O objetivo é garantir que, independentemente da ordem de execução das tarefas, os dados da conta permanecem coerentes.

Aceda à diretoria com o comando:

```sh
cd lab_sincronizacao
```

**1.1.** Compreender o programa

Abra o ficheiro `shared.c` no editor de texto à sua escolha e estude o seu conteúdo.  
Antes de executar o programa, responda:

**a)** O que representa o argumento passado ao programa na função `main`?

**b)** Que função vai executar a tarefa da Alice e a do Bob?

**c)** Que dados são partilhados?

**1.2.** Compile o programa.

Execute-o passando diferentes valores como argumento.

```sh
make
./shared 100
```

Experimente executar o programa com `1000` e valores superiores.

Para cada valor, repita a execução algumas vezes e compare os resultados.

**a)** Tente encontrar duas execuções com o mesmo argumento em que o Bob consiga levantar montantes diferentes. 
Como explica esta diferença?

**b)** Tente encontrar uma execução em que o saldo final da conta não corresponda ao total depositado pela Alice menos o total levantado pelo Bob. 
Como explica este resultado?

**c)** Tente encontrar uma execução em que o número de operações registado na conta não corresponda à soma das operações realizadas pela Alice e pelo Bob. 
O que aconteceu?

**1.3.** Ativar o _ThreadSanitizer_.

Pretende-se adicionar a *flag* de compilação `-fsanitize=thread` na `Makefile`.
Existe uma linha comentada preparada para esse efeito.

Recompile e execute novamente o programa:

```sh
make clean all
./shared 1000
```

Analise os diagnósticos apresentados pelo *ThreadSanitizer*.

**a)** Que dados partilhados aparecem nos diagnósticos?

**b)** Que acessos concorrentes são assinalados?

**1.4.** Identifique as secções críticas neste programa.

Uma secção crítica é uma região de código que acede a dados partilhados e que necessita de sincronização para impedir que acessos concorrentes incompatíveis deixem esses dados num estado inconsistente.

Para cada secção crítica, indique:

- os dados partilhados a que acede;
- se esses dados são apenas lidos ou também modificados.

## 2. Trinco lógico (*mutex*)

Um trinco lógico (*mutex*, de *mutual exclusion*, exclusão mútua em português) é um mecanismo de sincronização que permite a apenas uma tarefa de cada vez aceder a uma secção crítica.
Cada tarefa deve adquirir o trinco antes de aceder aos dados partilhados e libertá-lo quando termina.
Enquanto o trinco estiver ocupado, as outras tarefas que tentem adquiri-lo ficam à espera, evitando acessos simultâneos que possam tornar os dados incoerentes.

Pode consultar a documentação de [pthread_mutex_lock](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html) para saber mais detalhes.

**2.1.** Crie uma nova versão do programa no ficheiro `shared_mutex.c`.
Atualize a `Makefile`.

Resolva o problema de sincronização existente utilizando um trinco lógico.

**a)** Pode declarar e inicializar o trinco da seguinte forma:

```c
    pthread_mutex_t trinco;
    pthread_mutex_init(&trinco, NULL);
```

Ou, se o trinco for global, da seguinte forma numa só linha:

```c
pthread_mutex_t trinco = PTHREAD_MUTEX_INITIALIZER;
```

**b)** Use as funções `pthread_mutex_lock` à entrada e `pthread_mutex_unlock` à saída para sincronizar as secções críticas que identificou.

**c)** Compile e execute repetidamente o programa com diferentes valores. 
Confirme que os resultados se mantêm coerentes.

**2.2.** Execute novamente o programa com o *ThreadSanitizer*.

Confirme que este já não assinala os problemas de sincronização corrigidos.

## 3. Tarefas e trinco de leitura-escrita (*rwlock*)

O *mutex* permite garantir a exclusão mútua, mas limita a concorrência em situações em que algumas das tarefas pretendiam apenas consultar dados.

Um trinco de leitura-escrita (*read-write lock*) coordena o acesso a dados partilhados de forma a permitir que várias tarefas os possam ler em simultâneo, mas exige acesso exclusivo quando uma tarefa os pretende alterar.
Enquanto um escritor detém o trinco, nenhuma outra tarefa pode ler ou escrever os dados protegidos.
Este mecanismo pode melhorar o desempenho quando as leituras são mais frequentes do que as escritas.

Para saber mais sobre trincos de leitura-escrita (*rwlock*) pode [consultar o manual](https://man7.org/linux/man-pages/man3/pthread_rwlock_init.3p.html).

**3.1.** Crie uma nova versão do programa no ficheiro chamado `shared_rwlock.c`.
Atualize a `Makefile`.

Acrescente agora 4 tarefas (*threads*) que, no seu ciclo, se limitam a chamar a função `account_print_info`.

No ciclo da Alice e do Bob, acrescente também uma chamada à mesma função no final de cada iteração.

Com este novo programa, a função `account_print_info` passa a ser aquela que é mais frequentemente executada no programa.
Note também que é uma função que apenas lê dados partilhados, ou seja, nunca modifica dados partilhados.
Assim sendo, o programa é um bom candidato a beneficiar do uso de um trinco de leitura-escrita (*read-write lock*), em vez de um *mutex*.


**3.2.** Desenvolva um esquema de sincronização baseado em *read-write locks* que permita que o maior número possível de tarefas possa executar em paralelo.

Pode declarar um trinco deste novo tipo da seguinte forma:

```c
    pthread_rwlock_t rwl;
    pthread_rwlock_init(&rwl, NULL);
```

Passe a usar as funções `pthread_rwlock_rdlock` (aceder para leitura) e `pthread_rwlock_wrlock` (aceder para escrita) sempre que se iniciar uma secção crítica de leitura-apenas ou uma secção crítica em que haja pelo menos uma escrita a dados partilhados (respetivamente).

Em ambos os casos, liberte o trinco com `pthread_rwlock_unlock.`

**3.3.** Observe como muda o tempo de execução do programa ao usar *mutexes* e *rwlocks*.

Experimente (i) simular atrasos crescentes no acesso em leitura/escrita à conta chamando a função [`sleep`](https://man7.org/linux/man-pages/man3/sleep.3.html) dentro das secções críticas e (ii) alterar o número de tarefas que consultam a conta.

Em que situações consegue observar ganhos de desempenho para a solução baseada em *rwlocks*?

## Conclusão

Uma secção crítica executada por uma tarefa acede a dados partilhados e necessita de sincronização adequada para preservar a coerência desses dados.

Um *mutex* fornece acesso exclusivo à região protegida: apenas uma tarefa pode executar essa região de cada vez.

Um *read-write lock* distingue acessos de leitura e de escrita, o que pode permitir vários leitores em simultâneo, mas apenas um escritor de cada vez, sem leitores ativos.
Esta abordagem pode aumentar a concorrência quando as operações de leitura são significativamente mais frequentes do que as operações de escrita.

----

Contactos para sugestões/correções: [LEIC-Alameda](mailto:leic-so-alameda@disciplinas.tecnico.ulisboa.pt), [LEIC-Tagus](mailto:leic-so-tagus@disciplinas.tecnico.ulisboa.pt), [LETI](mailto:leti-so-tagus@disciplinas.tecnico.ulisboa.pt)
