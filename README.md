# Guião 5: Sincronização de secções críticas

![IST](img/IST_DEI.png)  

## Objectivos

No final deste guião, deverá ser capaz de:

- Saber identificar secções criticas em programas que utilizam threads.
- Resolver problemas de sincronização de secções criticas usando as interfaces POSIX `mutex` e `read-write lock`

Nota: Os tutoriais práticos de SO consistem num conjunto de exercícios práticos que permitem aos alunos familiarizarem-se com um determinado tema que será necessário para resolver os projetos da disciplina. Os tutoriais podem ser resolvidos individualmente ou em grupo. A sua resolução é recomendada mas não obrigatória. Não são avaliados. Cada tutorial pressupõe que os exercícios são realizados numa interface de linha de comandos (shell) de um sistema Unix/Linux ou equivalente. Assume também que os alunos já resolveram os tutoriais anteriores.

## Requisitos

- Sistema operativo Linux 20.04 LTS (se não o tiverem disponível no vosso computador pessoal, podem utilizar os computadores do laboratório);

## A Relembrar
Visite o guião de Deteção de Erros onde se encontra uma secção sobre o uso de sanitizadores de código. Como iremos lidar com programas concorrentes aconselha-se a utilização do ThreadSanitizer. Para isto, note a ativação deste sanitizador com a opção -fsanitize na Makefile, para já vamos deixar este sanitizador em comentário.

## Tarefas e trinco lógico (mutex)

Clone este repositório, usando o git: `git clone https://github.com/tecnico-so/lab_sincronizacao-seccoes-criticas.git`.

Aceda à diretoria com o comando:

```sh
cd lab_sincronizacao-seccoes-criticas
```

1. Abra o programa conta_partilhada.c no editor de texto á sua escolha, por exemplo, o vscode e estude o seu conteúdo.
    ```sh
    cd src/
    code contapartilhada.c
    ```
2. Compile este programa. Execute-o passando diferentes valores como argumento.
Experimente com 100, 1000, 10000 e valores superiores.
Para cada valor, experimente repetir a execução algumas vezes e observe se o resultado impresso é o mesmo.
    - Tente encontrar um exemplo em que, correndo duas vezes passando argumento idêntico, o Bob acaba por levantar diferentes montantes. Como explica este fenómeno?
    - Tente agora encontrar um exemplo em que o saldo final da conta não reflete o total depositado pela Alice subtraído pelo total gasto pelo Bob. Como explica este caso mais grave?
3. Identifique as secções críticas neste programa.
4. Resolva o problema de sincronização existente utilizando um trinco lógico (mutex).
    - Para saber mais sobre trincos lógicos pode consutar o manual [link](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html).
    - Pode declará-lo e inicializá-lo da seguinte forma:
        ```
            pthread_mutex_t trinco;
            pthread_mutex_init(&trinco, NULL);
    - Ou simplesmente:
        ```
        pthread_mutex_t trinco = PTHREAD_MUTEX_INITIALIZER;
        ```
    - De seguida, use as funções pthread_mutex_lock e pthread_mutex_unlock para sincronizar as secções críticas que identificou.
    - Compile e experimente o programa de maneira a confirmar que o erro grave que detetou na 2.b já não se verifica, para tal, pode ativar o sanitizador ThreadSanitizer tirando a opção -fsanitize de comentário no Makefile.

## Tarefas e trinco de leitura-escrita (rwlock)

1. No mesmo programa, acrescente agora 4 tarefas (threads) que, no seu ciclo, se limitam a chamar a função consultar_conta. No ciclo da Alice e Bob, acrescente também  uma chamada à mesma função no final de cada iteração.
    - Caso não tenha antes sincronizado a secção crítica em consultar_conta (pois essa função antes não era invocada concorrentemente), lembre-se que agora tem de o fazer!
    - Com este novo programa, a função consultar_conta passa a ser aquela que é mais frequentemente executada no programa. Nota também que é uma função que apenas lê sobre dados partilhados (ou seja, nunca modifica dados partilhados). Assim sendo, o programa é um bom candidato a beneficiar do uso de um trinco de leitura-escrita (rwlock), em vez de um mutex.
    - Para saber mais sobre trincos de leitura-escrita (rwlock) pode consutar o manual [link](https://man7.org/linux/man-pages/man3/pthread_rwlock_init.3p.html).
    - Desenvolva um esquema de sincronização baseado em read-write locks que
    permita que o máximo número de tarefas possa executar em paralelo.
        - Para tal, passe a declarar um trinco deste novo tipo:
            ```
            pthread_rwlock_t rwl;
            pthread_rwlock_init(&rwl, NULL);
            ```
        - E passe a usar as funções pthread_rwlock_rdlock e pthread_rwlock_wrlock sempre que se iniciar uma secção crítica de leitura-apenas ou uma secção crítica em que haja pelo menos uma escrita a dados partilhados (respetivamente).
        Em ambos os casos, liberte as secções críticas com
        pthread_rwlock_unlock.
    - Observe como muda o tempo de execução do programa ao usar mutexes e rwlocks.
        - Experimente (i) simular atrasos crescentes no acesso em leitura/escrita à conta chamando a função sleep dentro das secções críticas e (ii) alterar o número de tarefas que executam consultar_conta().
        - Em que situações consegue observar ganhos de desempenho para a
        solução baseada em rwlocks?


## Conclusão

Concluindo este guião já sabes como sincronizar secções criticas usando mutexes e read-write locks.
