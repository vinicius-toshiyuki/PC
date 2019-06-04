#include <stdio.h>
#include <sys/ioctl.h>
#include <wchar.h>
#include <locale.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NF 5 // Número de filósofos
#define BG(cor) "\e[48;5;" #cor "m"
#define FG(cor) "\e[38;5;" #cor "m"
#define CLEAR "\e[0m"
#define CLEARN "\e[0m\n"

sem_t garfos[NF];

void * filosofo(void *arg);

int comp(const void *a, const void *b){ return 1 - rand() % 3; }
char *filosofos[] = {
	"Abraham Joshua Heschel",
	"Adam Müller",
	"Alan Carter",
	"Alan Ryan",
	"Alastair Norcross",
	"Alexis de Tocqueville",
	"Alfred Rosenberg",
	"Alon Ben-Meir",
	"Andrei Marga",
	"António Castanheira Neves",
	"Antonio Negri",
	"Archeio-Marxism",
	"Archon",
	"Aristotle",
	"Armin Mohler",
	"Arthur Linton Corbin",
	"Arthur Moeller van den Bruck",
	"Auberon Herbert",
	"Averroes",
	"Axel Honneth",
	"Ayn Rand",
	"Benjamin Tucker",
	"Bertrand de Jouvenel",
	"Bertrand Russell",
	"Boris Furlan",
	"Bruno Leoni",
	"Bryan Caplan",
	"Carl Joachim Friedrich",
	"Carl Schmitt",
	"Carlo Lottieri",
	"Charles Blattberg",
	"Charles Taylor",
	"Cheng Hao",
	"Cheng Yi",
	"Claude Lefort",
	"Claude Lévi-Strauss",
	"Claudio Canaparo",
	"Confucius",
	"Constantin Rădulescu-Motru",
	"Cornel West",
	"Cornelius Castoriadis",
	"Costas Douzinas",
	"Dana Ward",
	"Daniel Guérin",
	"David D. Friedman",
	"David Kolb",
	"David Miller",
	"David Prychitko",
	"Dimitris Dimitrakos",
	"Disciples of Confucius",
	"Doctrine of the Mean",
	"Dong Zhongshu",
	"Duncan Kennedy",
	"Emma Goldman",
	"Ernest Wamba dia Wamba",
	"Ernst Bloch",
	"Étienne de La Boétie",
	"Eugen Rosenstock-Huessy",
	"Félix Guattari",
	"Felix Kaufmann",
	"Francesco D'Andrea",
	"Francis Fukuyama",
	"Francis Parker Yockey",
	"Frank Meyer",
	"Friedrich Hayek",
	"Fujiwara Seika",
	"Gene Callahan",
	"Georg Jellinek",
	"Georg Wilhelm Friedrich Hegel",
	"George Kateb",
	"George Ohsawa",
	"Georges Bataille",
	"Gerald Cohen",
	"Gerald Dworkin",
	"Giacomo Marramao",
	"Giambattista Vico",
	"Gianfranco Sanguinetti",
	"Gilles Deleuze",
	"Giorgio Agamben",
	"Giorgio Del Vecchio",
	"Gottfried Wilhelm Leibniz",
	"Gustave de Molinari",
	"Guy Aldred",
	"Guy Debord",
	"H. B. Acton",
	"H. L. A. Hart",
	"Han Fei",
	"Han Ryner",
	"Hannah Arendt",
	"Hans Achterhuis",
	"Hans Kelsen",
	"Hans Köchler",
	"Har Dayal",
	"Harriet Taylor Mill",
	"Hayashi Razan",
	"Henri Lefebvre",
	"Henry David Thoreau",
	"Henry George",
	"Henry Home, Lord Kames",
	"Henry Pachter",
	"Henry Sidgwick",
	"Herbert Spencer",
	"Herman Oliphant",
	"Hirata Atsutane",
	"Houston Stewart Chamberlain",
	"Howard Zinn",
	"Hugo Grotius",
	"Iain King",
	"Igor Pribac",
	"Immanuel Kant",
	"Iris Marion Young",
	"Isabel Paterson",
	"Isaiah Berlin",
	"Itō Jinsai",
	"Ivan Sviták",
	"J. J. C. Smart",
	"Jack Russell Weinstein",
	"James Brusseau",
	"James Tully",
	"Jamie Whyte",
	"Janet Biehl",
	"Janet Coleman",
	"Jawaharlal Nehru",
	"Jean Bodin",
	"Jean-François Lyotard",
	"Jean-Jacques Rousseau",
	"Jeremy Bentham",
	"John Austin",
	"John Burnheim",
	"John Finnis",
	"John Hospers",
	"John Locke",
	"John Oswald",
	"John Rawls",
	"John Searle",
	"John Stuart Mill",
	"John William Miller",
	"John Zerzan",
	"Jonathan Wolff",
	"Joseph de Torre",
	"Joseph Priestley e Dissidência",
	"Joseph Priestley",
	"Joseph Raz",
	"Joshua Cohen",
	"Judith Butler",
	"Julian Gumperz",
	"Julien Offray de La Mettrie",
	"Julius Binder",
	"Jürgen Habermas",
	"Kaibara Ekken",
	"Karl Loewenstein",
	"Karl Marx",
	"Karl Popper",
	"Karl-Otto Apel",
	"Kevin Carson",
	"Langdon Winner",
	"Leonard Borgzinner",
	"Leonard Read",
	"Leonidas Donskis",
	"Lew Rockwell",
	"Lewis Call",
	"Li",
	"Li",
	"Lon L. Fuller",
	"Loren Lomasky",
	"Lorenzo Peña",
	"Louis Althusser",
	"Ludwig von Mises",
	"Lysander Spooner",
	"Manuel De Landa",
	"Mao Zedong",
	"Margaret Canovan",
	"Martha Nussbaum",
	"Martin Luther King, Jr.",
	"Maurice Brinton",
	"Maurice Cranston",
	"Max Baginski",
	"Max Horkheimer",
	"Max Stirner",
	"Mazdak",
	"Mencius",
	"Michael Davis",
	"Michael Novak",
	"Michael Otsuka",
	"Michel Foucault",
	"Michel Onfray",
	"Mihailo Marković",
	"Mikelis Avlichos",
	"Mikhail Bakunin",
	"Milan Kangrga",
	"Mohandas Karamchand Gandhi",
	"Monarchomachs",
	"Mortimer Adler",
	"Mozi",
	"Murray Bookchin",
	"Murray Rothbard",
	"Nestor Makhno",
	"Niccolò Machiavelli",
	"Nigel Warburton",
	"Noam Chomsky",
	"Norbert Bolz",
	"Norbert Leser",
	"Octave Mirbeau",
	"Ogyū Sorai",
	"Oliver Wendell Holmes, Jr.",
	"Oskar Negt",
	"Paul R. Patton",
	"Paul Virilio",
	"Per Bauhn",
	"Peter Kropotkin",
	"Peter Lamborn Wilson",
	"Phaleas of Chalcedon",
	"Philip Mazzei",
	"Philip Pettit",
	"Pierre Bourdieu",
	"Pierre-André Taguieff",
	"Plato",
	"Rada Iveković",
	"Rainer Forst",
	"Raya Dunayevskaya",
	"Richard M. Weaver",
	"Right Hegelians",
	"Robert Nozick",
	"Robert P. George",
	"Robert Paul Wolff",
	"Roberto Mangabeira Unger",
	"Roderick Long",
	"Ronald Dworkin",
	"Russell Kirk",
	"Serge Moscovici",
	"Shmuel Alexandrov",
	"Simone Weil",
	"Slavoj Žižek",
	"Spencer Heath",
	"Stefan Molyneux",
	"Stephen Bronner",
	"Sun Yat-sen",
	"T. M. Scanlon",
	"Takis Fotopoulos",
	"Tan Sitong",
	"Tara Smith",
	"Theodor Sternberg",
	"Theodor W. Adorno",
	"St. Tomás de Aquino",
	"Thomas Hill Green",
	"Thomas Hobbes",
	"Thomas Nagel",
	"Þorsteinn Gylfason",
	"Thucydides",
	"Todd May",
	"Toju Nakae",
	"Tony Honoré",
	"Tripp York",
	"Voltaire",
	"Voltairine de Cleyre",
	"Walter Benjamin",
	"Warren Buffett",
	"Wesley Newcomb Hohfeld",
	"Wilhelm Dilthey",
	"William Fontaine",
	"William James",
	"William Paley",
	"William Sweet",
	"Wu Enyu",
	"Xun Zi",
	"Yamaga Sokō",
	"Yamazaki Ansai",
	"Yi I",
	"Young Hegelians",
	"Zhuangzi"
};

int main(int argc, char **argv){
	srand(time(NULL));
	qsort(filosofos, sizeof(filosofos) / sizeof(char *), sizeof(char *), comp);
	pthread_t thds[NF];
	int i;
	for(i = 0; i < NF; i++){
		sem_init(&garfos[i], 0, 1);
		int *id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&thds[i], NULL, filosofo, (void *) id);
	}
	pthread_join(thds[0], NULL);
	for(i = 0; i < NF; i++){
		sem_destroy(&garfos[i]);
	}
	return 0;
}
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void * filosofo(void *arg){
	int id = *((int *) arg);
	free(arg);
	while(1){
		// Pega dois garfos, da direita e da esquerda, e come
		int r = rand() % 2, teste = 0;
		pthread_mutex_lock(&mutex);
		while(sem_trywait(&garfos[id +  r]) || (++teste &&	sem_trywait(&garfos[(id + 1 - r) % NF]))){
			if(teste)
				sem_post(&garfos[id + r]), teste--;
			printf("[%02d] %-30s: " BG(73) "A vida não é competição, é cooperação: esperarei liberarem garfos." CLEARN, id, filosofos[id % (NF + 1)]);
			pthread_cond_wait(&cond, &mutex);
			int r = rand() % 2;
		}
		pthread_mutex_unlock(&mutex);
		printf("[%02d] %-30s: " BG(38) "Não vivemos para comer, mas comemos para viver.. Vou comer." CLEARN, id, filosofos[id % (NF + 1)]);
		sleep(2);
		printf("[%02d] %-30s: " BG(84) "A realidade é dura, mas é o único lugar onde se tem um bom bife.. Terminei de comer." CLEARN, id, filosofos[id % (NF + 1)]);
		sem_post(&garfos[id + r]);
		sem_post(&garfos[id + 1 - r]);
		pthread_cond_broadcast(&cond);
		printf("[%02d] %-30s: " BG(134) "Onde quer que vá, vá com o coração. Eu vou pensar." CLEARN, id, filosofos[id % (NF + 1)]);
		// Ao terminar de comer, libera os garfos e pensa
		sleep(5);
	}
	pthread_exit(NULL);
}


