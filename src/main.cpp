#include "modelo.h"
#include "exemplo.h"

int main(int argc, char const *argv[]) {

    if (argc < 3) {
        std::cout << "[Erro] - numero de parametros invalidos" << std::endl;
		std::cout << "[Uso] - ./exe instancia modelo" << std::endl;
		return -1;
    }

    Instancia instancia;

    if (atoi(argv[1]) == 1) {
        instancia = problemaUm();
    } else if (atoi(argv[1]) == 2) {
        instancia = problemaDois();
    } else {
        instancia = problemaTres();
    }
    
    try	{
		// Create an environment
		GRBEnv env = GRBEnv(true);
		env.set("LogFile", "fermat-weber.log");
		env.start();

        GRBModel model = atoi(argv[2]) == 1
            ? criarModelo(env, instancia.coordenadas, instancia.arestas)
            : criarModeloMaculan(env, instancia.coordenadas.size(), instancia.coordenadas);

        model.write("model.lp");

        model.optimize();

        std::cout << "x_{5,1} " << model.getVarByName("x_{5,1}").get(GRB_DoubleAttr_X) << std::endl;
        std::cout << "x_{5,2} " << model.getVarByName("x_{5,2}").get(GRB_DoubleAttr_X) << std::endl;
        std::cout << "x_{6,1} " << model.getVarByName("x_{6,1}").get(GRB_DoubleAttr_X) << std::endl;
        std::cout << "x_{6,2} " << model.getVarByName("x_{6,2}").get(GRB_DoubleAttr_X) << std::endl;

    } catch (GRBException e) {
		std::cout << "Error code = " << e.getErrorCode() << std::endl;
		std::cout << e.getMessage() << std::endl;
	} catch (...) {
		std::cout << "Exception during optimization" << std::endl;
	}


    return 0;
}
