#include "modelo.h"
#include "exemplo.h"

int main(int argc, char const *argv[]) {

    if (argc < 2) {
        std::cout << "[Erro] - numero de parametros invalidos" << std::endl;
		std::cout << "[Uso] - ./exe instancia" << std::endl;
		return -1;
    }

    Instancia instancia;

    if (atoi(argv[1]) == 1) {
        instancia = problemaUm();
    } else {
        instancia = problemaDois();
    }
    
    try	{
		// Create an environment
		GRBEnv env = GRBEnv(true);
		env.set("LogFile", "augmentation.log");
		env.start();

        GRBModel model = criarModelo(env, instancia.coordenadas, instancia.arestas);

    } catch (GRBException e) {
		std::cout << "Error code = " << e.getErrorCode() << std::endl;
		std::cout << e.getMessage() << std::endl;
	} catch (...) {
		std::cout << "Exception during optimization" << std::endl;
	}


    return 0;
}
