# 🌙 NieR: 2D - Uma Jornada Pixelada sob a Lua

## ✨ Sobre o Jogo

NieR: 2D 🌙 é uma reimaginação do universo de NieR, transportado para um envolvente estilo 2D pixel art. Desenvolvido em C com a biblioteca Raylib, fazendo uso de uma interface gráficas e enriquecido com a inteligência da Gemini AI, o jogo te coloca na pele de nossa protagonista em uma jornada misteriosa e emocionante sob o brilho da lua. Prepare-se para explorar mundos cativantes, derrotar autômatos maléficos e influenciada pela criatividade da inteligência artificial.

## 💻 Projeto com Raylib + cURL + Gemini API

Este repositório abriga o coração de NieR: 2D 🌙, um projeto construído com a robustez da linguagem C e a versatilidade da biblioteca gráfica Raylib ([https://www.raylib.com/](https://www.raylib.com/)). Para adicionar uma camada extra de dinamismo e surpresa, o jogo integra-se com a API Gemini ([https://ai.google.dev/](https://ai.google.dev/)), permitindo experiências de jogo únicas e adaptáveis.

A estrutura do projeto já está configurada, incluindo as dependências essenciais como `cURL` para comunicação de rede e `cJSON` para manipulação de dados JSON, facilitando a interação com a API Gemini.

## ⚙️ Requisitos do Sistema

Este jogo foi desenvolvido e testado no seguinte ambiente:

- **Sistema Operacional:** Windows
- **Biblioteca Gráfica:** [Raylib](https://www.raylib.com/) (baixada manualmente)
- **Ambiente de Desenvolvimento:** Acesso ao terminal `w64devkit` (incluso na pasta de instalação da Raylib)

## 🚀 Primeiros Passos: Como Jogar

Siga estes passos para compilar e rodar NieR: 2D 🌙 no seu sistema:

### 1. Baixe a Biblioteca Raylib

Para começar, você precisará da biblioteca Raylib instalada no seu sistema.

🔗 Acesse o site oficial: [https://www.raylib.com/](https://www.raylib.com/)

- Faça o download da versão para **Windows**.
- Extraia os arquivos baixados para o local de sua preferência (por exemplo, `C:\raylib`).
- Navegue até a pasta `/c/raylib/w64devkit` dentro dos arquivos extraídos. Este diretório contém as ferramentas de compilação necessárias.

### 2. Crie um Atalho do Terminal `w64devkit`

Este terminal fornece o ambiente de desenvolvimento correto para compilar o jogo.

- Dentro da pasta `w64devkit`, procure o arquivo executável chamado `w64devkit.exe`.
- Clique com o botão direito neste arquivo e selecione "Criar atalho".
- Arraste o atalho recém-criado para a sua **área de trabalho** para facilitar o acesso futuro.

### 3. Integre sua Chave da API Gemini

Para desbloquear as funcionalidades alimentadas pela inteligência artificial no jogo, você precisará fornecer sua chave de API Gemini.

- Abra o arquivo `src/gemini.c` com um editor de texto de sua preferência.
- Localize a seguinte seção de código:

```c
#define API_KEY "SUA_CHAVE_AQUI"
```

  - Substitua a string `"SUA_CHAVE_AQUI"` pela sua chave de API Gemini real. Você pode criar e gerenciar suas chaves no site do Google AI for Developers ([https://ai.google.dev/](https://ai.google.dev/)). **Lembre-se de manter sua chave de API confidencial e não a compartilhe em locais públicos.**

### 4. Compile e Execute o Jogo

Agora que tudo está configurado, você pode compilar e iniciar sua aventura em NieR: 2D 🌙!

  - Abra o terminal `w64devkit` (utilize o atalho que você criou na área de trabalho).
  - Use o comando `cd` para navegar até o diretório raiz do seu projeto NieR: 2D 🌙 (a pasta que contém o arquivo `Makefile`). Por exemplo, se o seu projeto estiver em `C:\projetos\nier2d`, você digitaria `cd C:\projetos\nier2d` e pressionaria Enter.
  - Uma vez dentro do diretório do projeto, execute o seguinte comando para compilar o código e rodar o jogo:

```bash
make run
```
## 🔩 Algoritmos e Estrutura de Dados Usados

No projeto NieR:2D usamos o método de ordenação Bubble Sort V0, no qual é usado a ordenação sem uso de Flags e feitas comparações e trocas de forma unidirecional, do começo ao fim. Fizemos uso dela justamente por ter um número baixo de recordes gravados no arquivo txt Rankings, sendo limitado apenas a 10 recordes. Já quando falamos de algoritmos de estrutura de dados, fizemos uso de listas duplamente encadeadas nas fases, ou salas do jogo. Permitindo ao player que avance até a última sala, onde encontrará o final boss. Para fazer a verificação dentro de jogo que é de fato duplamente encadeada, basta apenas, quando matar o inimigo, retornar a fase, anterior que o cenário anterior vai reaparecer normalmente.

Prepare-se para uma experiência única, onde o mundo de NieR ganha vida em pixels, com toques de inteligência artificial que podem surpreender a cada jogada!


## 👩‍💻 Membros

<ul>
  <li>
    <a > Arthur Xavier - 
    axrm@cesar.school 📩
  </li>
  <li>
    <a > Bruno Mayer -
    bbm@cesar.school 📩
  </li>
  <li>
      <a >  Caio Melo  -
    camm@gmail.com 📩
  </li>
  <li>
    <a >  Erick Belo -
    eab2@gmail.com 📩
  </li> 
        <li>
    <a >  Guilherme Rangel -
     gvrs@cesar.school 📩
  
