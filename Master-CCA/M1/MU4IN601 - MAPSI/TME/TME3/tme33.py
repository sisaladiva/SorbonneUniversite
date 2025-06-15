import numpy as np
import matplotlib.pyplot as plt

def learnML_parameters(X_train,Y_train):
    mu = []
    sig = []
    for i in range(10):
        moy=X_train[Y_train==i].mean(0)
        mu.append(moy)
        sigm=X_train[Y_train==i].std(0)
        sig.append(sigm)
    return np.array(mu), np.array(sig)



def log_likelihood ( X_train, mu, sig, defeps):
    somme=0
    for i in range(len(X_train)):
        if defeps>0:
            sig= np.maximum(sig,defeps)
            somme=somme+np.log(2*np.pi*sig[i]**2)+(((X_train[i]-mu[i])**2)/((sig[i])**2))
        else:
            if (sig[i]>0):
                somme=somme+np.log(2*np.pi*sig[i]**2)+(((X_train[i]-mu[i])**2)/((sig[i])**2))
         
    vraisemblance= (-1/2)* somme
    return vraisemblance


def classify_image ( X_train, mu, sig, defeps):
    return np.argmax([log_likelihood(X_train, mu[i], sig[i], defeps) for i in range(10)])

#met longtemps pour s'executer , attendre environ 3-4 min je ne sais pas pourquoi 
def classify_all_images ( X_train, mu, sig, defeps):
    return np.array([classify_image(X_train[i],mu,sig,defeps) for i in range (len (X_train))])
    
def matrice_confusion (Y_train, Y_train_hat):
    matrice_confusion=np.zeros((10,10))
    for i in range (len(Y_train)):
        realite=Y_train[i]
        prediction=Y_train_hat[i]
        matrice_confusion[realite][prediction]=matrice_confusion[realite][prediction]+1
    return matrice_confusion


def classificationRate (Y_train, Y_train_hat):
    bonnes_predictions=np.sum(Y_train==Y_train_hat)
    tot=len(Y_train)
    return bonnes_predictions/tot


def classifTest(X_test,Y_test,mu,sig,e):
    print("1- Classify all test images...")
    Y_hat = classify_all_images(X_test, mu, sig, -1)
    m = matrice_confusion(Y_test, Y_hat)
    res=np.where( Y_test != Y_hat)

    taux = 1-np.mean(np.where(Y_test != Y_hat, 1, 0))
    print(f"2- Classification rate : {taux}")
    print("3- Matrice de confusion : ")
    plt.figure(figsize=(3,3))
    plt.imshow(m)
    return res

    
def binarisation (X):
    return np.where(X>0,1,0)

def learnBernoulli ( Xb_train,Y_train ):
    theta= [Xb_train [Y_train==i].mean(0) for i in range(10)]
    return np.array(theta)



def logpobsBernoulli( X_train, theta, epsilon):
    tclip = np.clip(theta, epsilon, 1-epsilon)
    A= X_train * np.log(tclip) + (1-X_train) * np.log(1 - tclip)
    res= A.sum(axis=1)
    return res


def classifBernoulliTest( Xb_test, Y_test, theta):
    print("1- Classify all test images...")
    tailleX=len(Xb_test)
    Y_hat=[]
    for i in range(tailleX):
        Y_hat.append(np.argmax(logpobsBernoulli(Xb_test[i], theta, 1e-4))) 
    m= matrice_confusion(Y_test, Y_hat)
    taux= np.where(Y_test == Y_hat, 1, 0).mean()
    print(f"2- Classification rate : {taux}")   
    print("3- Matrice de confusion : ")

    plt.figure()
    plt.imshow(m)

    return



def learnGeom( Xg_train, Yg_train,seuil=1e-4) :
    theta=[]
    n=16
    for i in range( 10):
        indices=np.where(Yg_train==i)[0]
        s=np.zeros(n)
        thetai=[]
        for ind in indices:
            s+= Xg_train[ind]
        for j in range(n):
            if s[j]==0:
                thetai.append(len(indices)/seuil)
            elif s[j]== len(indices):
                thetai.append(len(indices)/s[j])
            else:
                thetai.append(len(indices)/s[j])
        theta.append(thetai)
    return np.array(theta)
    

def logpobsGeom(Xg_train, theta):
    epsilon=1e-4
    theta_clip = np.clip(theta, epsilon, 1 - epsilon)
    return np.log((theta * (1 - theta) ** (Xg_train - 1)).prod(axis=1))

def classifyGeom( Xg_train, theta):
    return np.argmax(logpobsGeom(Xg_train, theta))
