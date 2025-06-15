import matplotlib.pyplot as plt
import numpy as np
def learnHMM(allX, allS, N, K):
    
    A = np.zeros((N, N)) 
    B = np.zeros((N, K)) 

    for i in range(1,len(allS)):
        A[allS[i-1]][allS[i]]+=1
        
    for i in range(len(allX)):
        B[allS[i]][allX[i]]+=1
    
    for i in range(len(A)) :
        A[i] = A[i] / A[i].sum()
    
    for i in range(len(B)) :
        B[i] = B[i] / B[i].sum()
     
    return A,B

def viterbi(allx,Pi,A,B):
    """
    Parameters
    ----------
    allx : array (T,)
        Sequence d'observations.
    Pi: array, (N,)
        Distribution de probabilite initiale
    A : array (N, N)
        Matrice de transition
    B : array (N, K)
        Matrice d'emission matrix

    """

    ## initialisation
    psi = np.zeros((len(A) , len(allx))) # A = N
    psi[:, 0] = -1
    delta = np.zeros((len(A), len(allx)))  
    delta[:, 0] = np.log(Pi) + np.log(B[:, allx[0]])
    liste=[]
    for i in delta:
        liste.append(i[0])
    liste=np.array(liste)
    print(liste)
    for t in range(1, len(allx)):
        for j in range(len(A)):
            delta[j, t] = np.max(delta[:, t-1] + np.log(A[:, j])) + np.log(B[j, allx[t]])
            psi[j, t] = np.argmax(delta[:, t-1] + np.log(A[:, j]))
        if (t%100000==0):
            print(f"t= {t}  delta[:,t]= {delta[:, t]}")
    S = np.zeros(len(allx), dtype=int)

    S[-1] = delta[:, -1].argmax()
    
    for t in range(len(allx)-2,-1,-1):
        S[t] = psi[S[t+1], t+1]
   
    return S 
    

def get_and_show_coding(etat, annot):
    etat[etat!=0]=1 
    annot[annot!=0]=1
    fig, ax = plt.subplots(figsize=(15,2))
    ax.plot(annot[100000:200000], label="annotation", lw=3, color="black", alpha=.4)
    ax.plot(etat[100000:200000], label="prediction", ls="--")
    plt.legend(loc="best")
    plt.show()
    etat[etat!=0]=1 
    annot[annot!=0]=1
    
    return etat, annot


def create_confusion_matrix(true_sequence, predicted_sequence):
    resultat = np.zeros((2,2))      
    for i in range(len(true_sequence)):
        if true_sequence[i] == 1:
            if predicted_sequence[i] == 1:
                resultat[0][0] +=1
            else:
                resultat[1][0] +=1
        else:
            if predicted_sequence[i] == 0:
                resultat[1][1] +=1
            else:
                resultat[0][1] +=1
            
    return resultat

def get_annoatation2(annotation):
    annotation2 = np.zeros(len(annotation), dtype=int)
    
    

    for i in range(0,len(annotation),1):

        if annotation[i]==1 and annotation[i-1]==0:
            annotation2[i]=1
            annotation2[i+1]=2
            annotation2[i+2]=3
        if ( annotation[i-1]==3 or annotation[i-1]==6 ) and annotation[i+1]!=0 and annotation[i+4]!=0 :
            annotation2[i]=4
            annotation2[i+1]=5
            annotation2[i+2]=6
        if annotation2[i-1] ==6 and (annotation[i+3]==0 or (i+2)== len(annotation)-1) and annotation2[i-1]!=9: 
            annotation2[i] = 7
            annotation2[i+1] = 8
            annotation2[i+2] = 9
        if annotation[i]==0:
            annotation2[i]=0
        
        
    return annotation2
