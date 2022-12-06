# Guião 5: Sincronização de secções críticas

![IST](img/IST_DEI.png)

## Objectivos

No final deste guião, deverá ser capaz de:

- saber identificar secções críticas em programas que utilizam _threads_;
- resolver problemas de sincronização de secções críticas usando as interfaces POSIX para _mutex_ e _read-write lock_.

## Requisitos

- Sistema operativo Linux Ubuntu 20.04 LTS (se não o tiverem disponível no vosso computador pessoal, podem utilizar os computadores do laboratório).

## A relembrar

Visite o [guião sobre deteção de erros](https://github.com/tecnico-so/lab_detecao-erros) onde são apresentados os sanitizadores de código.
Mais adiante, iremos utilizar o _ThreadSanitizer_ para ajudar com programas concorrentes.
Para ativar o este sanitizador, usar a opção `-fsanitize` na `Makefile` que, para já, está comentada.

## Tarefas e trinco lógico (_mutex_)

Clone este repositório, usando: `git clone https://github.com/tecnico-so/lab_sincronizacao.git`.

Aceda à diretoria com o comando:

```sh
cd lab_sincronizacao
```

1. Abra o programa `conta_partilhada.c` no editor de texto à sua escolha e estude o seu conteúdo.
2. Compile este programa. Execute-o passando diferentes valores como argumento.
Experimente com `100`, `1000`, `10000` e valores superiores.
Para cada valor, experimente repetir a execução algumas vezes e observe se o resultado impresso é o mesmo.
    - Tente encontrar um exemplo em que, correndo duas vezes passando argumento idêntico, o Bob acaba por levantar diferentes montantes.
    Como explica este fenómeno?
    - Tente agora encontrar um exemplo em que o saldo final da conta não reflete o total depositado pela Alice subtraído pelo total gasto pelo Bob.
    Como explica este caso mais grave?
    - Tente encontrar um exemplo em que o número de operações feitos na conta não é a soma do número de operações feitas pela Alice e das operações feitas pelo Bob.
3. Ligue o _ThreadSanitizer_, adicionando a flag `-fsanitize=thread` na `Makefile`.
Recompile o programa, com `make clean all`, e corra novamente.
O que consegue perceber do _output_ do sanitizador?
4. Identifique as secções críticas neste programa.
5. Resolva o problema de sincronização existente utilizando um trinco lógico, chamado _mutex_ (abreviatura de _mutual exclusion_).
    - Para saber mais sobre trincos lógicos pode [consultar o manual](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html).
    - Pode declará-lo e inicializá-lo da seguinte forma:

        ```c
            pthread_mutex_t trinco;
            pthread_mutex_init(&trinco, NULL);
        ```

    - Ou simplesmente, se o trinco for global:

        ```c
        pthread_mutex_t trinco = PTHREAD_MUTEX_INITIALIZER;
        ```

    - De seguida, use as funções `pthread_mutex_lock` e `pthread_mutex_unlock` para sincronizar as secções críticas que identificou.
    - Compile e experimente o programa de maneira a confirmar que o erro grave que detetou no ponto 2 <!-- 2.b --> já não se verifica, observando que o _ThreadSanitizer_ já não reporta nenhum problema.

## Tarefas e trinco de leitura-escrita (_rwlock_)

1. No mesmo programa, acrescente agora 4 tarefas (_threads_) que, no seu ciclo, se limitam a chamar a função `account_print_info`.
No ciclo da Alice e Bob, acrescente também uma chamada à mesma função no final de cada iteração.
    - Com este novo programa, a função `account_print_info` passa a ser aquela que é mais frequentemente executada no programa.
    Notr também que é uma função que apenas lê sobre dados partilhados, ou seja, nunca modifica dados partilhados.
    Assim sendo, o programa é um bom candidato a beneficiar do uso de um trinco de leitura-escrita (_read-write lock_), em vez de um _mutex_.
    - Para saber mais sobre trincos de leitura-escrita (_rwlock_) pode [consultar o manual](https://man7.org/linux/man-pages/man3/pthread_rwlock_init.3p.html).
    - Desenvolva um esquema de sincronização baseado em _read-write locks_ que permita que o máximo número de tarefas possa executar em paralelo.
        - Para tal, passe a declarar um trinco deste novo tipo:

            ```c
            pthread_rwlock_t rwl;
            pthread_rwlock_init(&rwl, NULL);
            ```

        - E passe a usar as funções `pthread_rwlock_rdlock` e `pthread_rwlock_wrlock` sempre que se iniciar uma secção crítica de leitura-apenas ou uma secção crítica em que haja pelo menos uma escrita a dados partilhados (respetivamente).
        Em ambos os casos, liberte as secções críticas com `pthread_rwlock_unlock.`
    - Observe como muda o tempo de execução do programa ao usar mutexes e rwlocks.
        - Experimente (i) simular atrasos crescentes no acesso em leitura/escrita à conta chamando a função [`sleep`](https://man7.org/linux/man-pages/man3/sleep.3.html) dentro das secções críticas e (ii) alterar o número de tarefas que consultam a conta.
        - Em que situações consegue observar ganhos de desempenho para a solução baseada em _rwlocks_?

## Conclusão

Uma secção crítica de um programa acede a um ou mais recursos partilhados por várias tarefas e necessita de ser protegida de modo a garantir o acesso em exclusão mútua, ou seja, é preciso garantir que apenas uma tarefa pode executar essa parte do código de cada vez.
Um mecanismo que permite garantir a exclusão mútua é um _mutex_.
Um outro mecanismo que também permite é o _rwlock_, com a diferença que permite diferenciar acessos de leitura e de escrita, tornando possível ter vários leitores na secção crítica ao mesmo tempo ou então apenas um escritor.

----

Contactos para sugestões/correções: [LEIC-Alameda](mailto:leic-so-alameda@disciplinas.tecnico.ulisboa.pt), [LEIC-Tagus](mailto:leic-so-tagus@disciplinas.tecnico.ulisboa.pt), [LETI](mailto:leti-so-tagus@disciplinas.tecnico.ulisboa.pt)
