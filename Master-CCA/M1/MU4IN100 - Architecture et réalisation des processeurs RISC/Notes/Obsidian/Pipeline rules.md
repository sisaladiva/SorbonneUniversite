# Pipeline rules (english)

1. Stages need to be time-balanced.
2. Data can only pass from one stage to another using only one register.
3. Every component belongs to one and only one stage (only one stage can **write** to the register bank)

# Pipeline rules (français)

Vérifier que les **règles du pipeline** sont respectées :
1. Chaque étape et opération doit être séparée par un registre : si non respecté, on ajoute des instructions de transfert ou on modifie le découpage
2. Chaque matériel est utilisé dans un unique étage : si non respectés, renommer les registres (en utiliser d'autres)
3. Réorganiser sous la forme $En_i, …, E0_{n+i}$
4. Effectuer le réordonnancement de la gestion de boucle
