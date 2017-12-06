# Especificação do Trabalho
## Alunos
- Marlon Baptista de Quadros
- Matheus Britzke

A partir do exercício anterior (Exercício 3 - análise sintática ascendente - YACC), altere sua especificação confome descrito abaixo:

- [x] Altere a estrutura sintática de uma função para permitir a declaração de variáveis locais.

Altere a especificação gerada para realizar as seguintes verificações de tipos:

- [x] todo identificador deve ser declarado antes do seu uso
- [ ] assim como no Java, variáveis globais possuem valor default, variáveis locais, no entanto, devem ter sido inicializadas antes do seu uso
- [x] operadores aritméticos exigem operandos numéricos (int ou double) e o resultado, como em java, será o maior deles (double > int)
- [x] operadores relacionais exigem operandos numéricos e o resultado é um valor lógico (boolean)
- [x] operadores lógicos exigem operandos lógicos e o resultado é um valor lógico
- [x] atribuição deve ser entre elementos do mesmo tipo, ou de um tipo numérico menor para um maior (int < double)
- [x] comandos de seleção e repetição exigem expressões lógicas
- [ ] na ativação de uma função deve ser validado o número e tipo dos parâmetros reais
- [ ] as mesmas regras de escopo das linguagens C/Java se aplicam, ou seja, variáveis globais (ou atributos no caso de Java) são visíveis em todas as funções e variáveis locais (e parâmetros) apenas na função onde foram declaradas. Lembre que variáveis locais/parâmetros "escondem" uma declaração global.

## Comandos
```
make clean
```
```
make
```
```
java -cp . Parser tests_success/success01.txt
```
