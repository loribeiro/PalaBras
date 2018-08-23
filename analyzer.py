import networkx as nx
import sys
def estatisticas(endereco,arq):
    arquivo = open(endereco+'/'+arq+'_estatisticas.csv', 'w')
    e= endereco+'/'+arq+'Convertido.redeFras.csv'
    graph = nx.read_edgelist(
       e)
    nx.to_directed(graph)
    clust_coeficients = nx.clustering(graph)
    dict_betweenes = nx.betweenness_centrality(graph)
    closeness = nx.closeness_centrality(graph)
    eigenvector_centrality = nx.eigenvector_centrality(graph)
    centralidade_grau = nx.degree_centrality(graph)
    grau = nx.degree(graph)
    #modularidade = nx.modularity_matrix(graph)
    #centro= nx.center(graph,e=None, usebounds=False)
    # diametro= nx.diameter(graph,e=None, usebounds=False)
    densidade = nx.density(graph)
    print densidade
    arquivo.write("No ; Grau ; Betweennes ; Centralidade_Grau ; Centralidade(Closeness) ; Eigenvector_Centrality ; Clustering\n")
    for key in dict_betweenes.keys():
        aux = str(dict_betweenes[key])
        aux1= str(grau[key])
        aux2= str(centralidade_grau[key])
        aux3= str(closeness[key])
        aux4= str(eigenvector_centrality[key])
        aux5= str(clust_coeficients[key])
        arquivo.write(key + ' ; '+aux1+' ; '+ aux +' ; '+aux2 +' ; '+aux3+' ; '+aux4+' ; '+aux+ '\n')
    arquivo.close()

ende=str(sys.argv[1])
arq=str(sys.argv[2])
estatisticas(ende,arq)

