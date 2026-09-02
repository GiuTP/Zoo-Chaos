<a id="readme-top"></a>

[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<br />
<div align="center">
  <h3 align="center">🦁 Zoo Chaos</h3>

  <p align="center">
    Um jogo de plataforma 2D autoral desenvolvido em C com Allegro 5 — trabalho final da disciplina Programação 2 (CI1002) na UFPR.
    <br />
    <a href="https://github.com/GiuTP/Zoo-Chaos/issues/new?labels=bug">Reportar Bug</a>
    &middot;
    <a href="https://github.com/GiuTP/Zoo-Chaos/issues/new?labels=enhancement">Sugerir Melhoria</a>
  </p>
</div>

---

<!-- SUMÁRIO -->
<details>
  <summary>Sumário</summary>
  <ol>
    <li><a href="#-sobre-o-projeto">Sobre o Projeto</a>
      <ul>
        <li><a href="#-construído-com">Construído com</a></li>
      </ul>
    </li>
    <li><a href="#-como-jogar">Como Jogar</a></li>
    <li><a href="#-personagens-e-inimigos">Personagens e Inimigos</a></li>
    <li><a href="#-mecânicas">Mecânicas</a></li>
    <li><a href="#-estrutura-do-projeto">Estrutura do Projeto</a></li>
    <li>
      <a href="#-instalação">Instalação</a>
      <ul>
        <li><a href="#-pré-requisitos">Pré-requisitos</a></li>
        <li><a href="#-compilação">Compilação</a></li>
      </ul>
    </li>
    <li><a href="#-dificuldades-e-aprendizados">Dificuldades e Aprendizados</a></li>
    <li><a href="#-licença">Licença</a></li>
    <li><a href="#-contato">Contato</a></li>
    <li><a href="#-agradecimentos">Agradecimentos</a></li>
  </ol>
</details>

---

## 🎮 Sobre o Projeto

![Imagem de demonstração de uma parte jogo](assets/screenshots/screenshot.png)

**Zoo Chaos** é um jogo de plataforma 2D inspirado em Super Mario, desenvolvido inteiramente em linguagem C como trabalho final da disciplina **Programação 2 (CI1002)** da **Universidade Federal do Paraná (UFPR)**.

O jogo se passa em um zoológico caótico onde o personagem principal — o próprio autor — precisa atravessar o mapa enquanto desvia e enfrenta os animais que escaparam dos recintos. Todas as artes do jogo são **autorais**, criadas especialmente para o projeto.

O objetivo é simples: **chegar ao extremo oposto do mapa**. Mas os animais no caminho não vão facilitar.

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

### 🛠 Construído com

* [![C][C-badge]][C-url]
* [![Allegro][Allegro-badge]][Allegro-url]

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 🕹 Como Jogar

| Tecla | Ação |
|-------|------|
| `A` / `D` | Mover para esquerda / direita |
| `J` | Pular |
| `S` | Abaixar (enquanto no chão) |
| `K` | Agarrar em cipó (enquanto no ar, próximo a um) |
| `J` (no cipó) | Saltar do cipó |
| `ESC` | Pausar / Despausar |
| `Q` (pausado) | Voltar ao menu principal |
| `Enter` | Confirmar seleção no menu |
| `W` / `S` ou `↑` / `↓` | Navegar no menu |

**Objetivo:** Alcance o extremo direito do mapa para vencer a fase.

**Vidas:** O personagem possui 3 corações. Ao perdê-los todos, o jogo reinicia do início.

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 🦔 Personagens e Inimigos

### Protagonista

O personagem principal é o próprio autor do jogo. Ele corre, pula, se abaixa e consegue se agarrar em cipós para atravessar obstáculos.

### Inimigos

Todos os inimigos são animais do zoológico, cada um com um comportamento distinto:

| Inimigo | Comportamento | Pode ser eliminado |
|---------|--------------|-------------------|
| **Abelha** | Estática, causa dano por contato | Não |
| **Tubarão** | Sobe e desce verticalmente em intervalos | Não |
| **Pássaro** | Voa em diagonal, muda de direção ao atingir os limites | Sim |
| **Pinguim** | Patrulha horizontalmente e desliza pelo chão | Sim |
| **Piranha** | Patrulha horizontalmente na água | Não |
| **Panda** | Patrulha e periodicamente rola — **invencível durante a rolagem** | Sim (fora da rolagem) |

> Inimigos marcados como "pode ser eliminado" são derrotados ao serem pisados por cima (queda sobre eles), assim como em Super Mario.

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## ⚙ Mecânicas

### Coleta de Moedas e Modo Estrela
Moedas estão espalhadas pelo mapa. Ao coletar a **quarta moeda**, o personagem entra no **modo estrela**: fica temporariamente invencível e pode eliminar qualquer inimigo por simples contato. O contador de moedas zera ao morrer ou reiniciar.

### Cipós
Cipós se balançam sozinhos pelo cenário. O personagem pode se agarrar a eles enquanto estiver no ar (tecla `K`) e usá-los para atravessar gaps. Pressionar `J` enquanto agarrado realiza um salto a partir do cipó.

### Abaixar
Ao pressionar `S` no chão, o personagem se abaixa, reduzindo sua hitbox verticalmente — útil para passar por obstáculos baixos e desviar de projéteis ou inimigos.

### Câmera
A câmera segue o personagem horizontalmente com rolagem suave, limitada às bordas do mapa.

### Sistema de Dano e Invencibilidade
Ao tomar dano, o personagem fica temporariamente invencível (efeito de piscar) por alguns segundos, evitando dano em cadeia.

### Música
A trilha sonora acelera automaticamente quando o personagem entra no modo estrela, retornando ao ritmo normal ao sair dele.

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 📁 Estrutura do Projeto

```
Zoo-Chaos/
├── assets/
│   ├── backgrounds/     sprites de background (bg0.png … bg6.png)
│   ├── entidades/       spritesheets do player e inimigos
│   ├── maps/            arquivos de mapa em texto (.txt)
│   ├── menus/           telas de menu, vitória e derrota
│   ├── plataformas/     tilesets de plataformas
│   └── soundtrack/      trilha sonora do jogo
├── include/
│   ├── engine.h         agregador de todos os headers do jogo
│   ├── entity.h         definição de Entity e EntitiesManager
│   ├── game.h           definição do loop principal (GAME)
│   ├── menu.h           definição do sistema de menus (MENU)
│   ├── player.h         definição do Player
│   └── world.h          definição do mundo e plataformas (WORLD)
├── src/
│   ├── main.c           ponto de entrada
│   ├── entity.c         lógica e IA dos inimigos
│   ├── game.c           inicialização, loop principal e câmera
│   ├── menu.c           renderização e controle dos menus
│   ├── player.c         movimentação, animação e colisão do player
│   └── world.c          carregamento e renderização do mundo
├── compile_flags.txt    configuração do clangd (LSP)
├── makefile
└── README.md
```

Os mapas de plataformas e entidades são carregados a partir de arquivos `.txt` em `assets/maps/`, permitindo editar o layout do nível sem recompilar.

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 🚀 Instalação

### 📦 Pré-requisitos

É necessário ter o **GCC** e a biblioteca **Allegro 5** instalados. No Ubuntu/Debian:

```sh
sudo apt install gcc make \
  liballegro5-dev \
  liballegro-image5-dev \
  liballegro-audio5-dev \
  liballegro-acodec5-dev \
  liballegro-primitives5-dev \
  liballegro-font5-dev
```

No Arch Linux:

```sh
sudo pacman -S gcc make allegro
```

### 🔧 Compilação

1. Clone o repositório:
   ```sh
   git clone https://github.com/GiuTP/Zoo-Chaos.git
   cd Zoo-Chaos
   ```

2. Compile:
   ```sh
   make
   ```

3. Execute a partir da **raiz do repositório** (necessário para que os assets sejam encontrados):
   ```sh
   ./bin/chaos_zoo
   ```

4. Para limpar os arquivos de compilação:
   ```sh
   make clean
   ```

> **Atenção:** o jogo deve ser executado sempre a partir da raiz do projeto, pois os caminhos dos assets são relativos a ela (`assets/...`).

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 📚 Dificuldades e Aprendizados

Este projeto foi um grande desafio e uma fonte enorme de aprendizado. As principais dificuldades enfrentadas foram:

- **Entender o Allegro 5** — a biblioteca tem muitos subsistemas (áudio, imagem, eventos, timers, bitmaps) e configurá-los corretamente levou tempo e muita leitura de documentação.
- **Lógica de gamedev** — pensar em loop de jogo, separação de update/draw, gerenciamento de estado (menu, playing, win, lose) e câmera foram conceitos novos que exigiram bastante estudo.
- **Sistema de animação de sprites** — entender como fatiar um spritesheet e controlar o avanço de frames por tempo (em vez de por tick) para obter animações suaves foi um dos pontos mais trabalhosos.
- **Transformações angulares** — implementar o balanço do cipó com `sin`/`cos` e fazer o pássaro voar em trajetória diagonal com reflexão nos limites envolveu matemática que foi desafiadora de depurar visualmente.

Apesar das dificuldades, o resultado final é um jogo completo, jogável, com artes 100% autorais — algo do qual tenho muito orgulho.

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 📄 Licença

O código-fonte deste projeto está distribuído sob a licença **MIT**. Consulte o arquivo `LICENSE` para mais informações.

As artes do jogo — sprites, backgrounds, menus e demais visuais — são de **autoria exclusiva** e não estão cobertas por esta licença.

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 📬 Contato

GiuTP — [github.com/GiuTP](https://github.com/GiuTP)

E-mail — giulianotpt@gmail.com

Link do projeto: [https://github.com/GiuTP/Zoo-Chaos](https://github.com/GiuTP/Zoo-Chaos)

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

## 🙏 Agradecimentos

* Minha irmã — responsável por todas as artes do jogo
* [Best-README-Template](https://github.com/othneildrew/Best-README-Template) — template base deste README

<p align="right">(<a href="#readme-top">voltar ao topo</a>)</p>

---

<!-- MARKDOWN LINKS & IMAGES -->
[stars-shield]: https://img.shields.io/github/stars/GiuTP/Zoo-Chaos.svg?style=for-the-badge
[stars-url]: https://github.com/GiuTP/Zoo-Chaos/stargazers
[issues-shield]: https://img.shields.io/github/issues/GiuTP/Zoo-Chaos.svg?style=for-the-badge
[issues-url]: https://github.com/GiuTP/Zoo-Chaos/issues
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/giuliano-tavares/
[C-badge]: https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white
[C-url]: https://en.wikipedia.org/wiki/C_(programming_language)
[Allegro-badge]: https://img.shields.io/badge/Allegro_5-darkred?style=for-the-badge
[Allegro-url]: https://liballeg.org/
