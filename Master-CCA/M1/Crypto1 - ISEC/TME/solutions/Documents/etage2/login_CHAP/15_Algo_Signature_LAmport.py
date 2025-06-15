# Signature de Lamport
# ====================

# Aucune RFC ne spécifie la signature de Lamport, car elle est rarement utilisée
# telle quelle.  Elle est néanmoins très facile à implanter.  Une implantation 
# open-source se trouve à la fin de ce document.

# Une clef (privée ou publique) se compose 512 chaines de 256 bits concaténées. 
# On note K[i] la i-ème de ces chaines, pour 0 <= i < 512.  Pour générer une 
# paire de clefs, on tire la clef secrète entièrement au hasard.  La clef publique
# est obtenue en posant : pk[i] = sha256(sk[i]).

# Une signature sig est une séquence de 256 chaînes de 64 caractères hexadécimaux.

# Pour signer un message M, on applique la procédure suivante :
# 1. [Hachage.]          Calculer : h <--- SHA256(M)
# 2. [Assemblage.]       Pour tout 0 <= i < 256:
#                            soit b le i-ème bit de h
#                            sig[i] <--- sk[2*i + b]

# Pour vérifier une signature S d'un message M, on effectue la procédure suivante :
# 1. [Hachage.]          Calculer : h <--- SHA256(M)
# 2. [Vérification.]     Pour tout 0 <= i < 256:
#                            soit b le i-ème bit de h
#                            si pk[2 * i + b] != sha256(S[i]), alors échouer


# IMPLANTATION EN PYTHON
# ----------------------
import base64
import textwrap
import secrets
from hashlib import sha256

class LamportKey:
    """
    Abstract Base Class for Lamport Keys (both public and private)
    """
    def __init__(self, key : bytes):
        self.key = [key[i:i+32] for i in range(0, 16384, 32)]

    @classmethod
    def _prefix(cls):
        return f"-----BEGIN {cls.KIND}-----"

    @classmethod
    def _suffix(cls):
        return f"-----END {cls.KIND}-----"

    def dumps(self) -> str:
        """
        Return a PEM representation of the key
        """
        payload = base64.b64encode(b''.join(self.key)).decode()
        middle = '\n'.join(textwrap.wrap(payload, width=64))
        return f"{self._prefix()}\n{middle}\n{self._suffix()}"

    @classmethod
    def loads(cls, key : str):
        """
        Return a key object from its string representation
        """
        prefix = cls._prefix()
        suffix = cls._suffix()
        if not key.startswith(prefix):
            raise ValueError("not a PEM-encoded key (missing prefix)")
        if not key.endswith(suffix):
            raise ValueError("not a PEM-encoded key (missing suffix)")
        payload = key[len(prefix):-len(suffix)]
        return cls(base64.b64decode(payload))

class LamportPrivateKey(LamportKey):
    KIND = 'LAMPORT SECRET KEY'

    @staticmethod
    def keygen():
        """
        Generate a fresh random private key
        """
        return LamportPrivateKey(secrets.token_bytes(512*32))

    def pk(self):
        """
        Return the public key associated to this private key
        """
        pk = b''
        for i in range(512):
            pk += sha256(self.key[i]).digest()
        return LamportPublicKey(pk)

    def sign(self, m : bytes) -> str:
        """
        Sign a message using this private key
        """
        sig = b''
        h = int.from_bytes(sha256(m).digest(), byteorder='big')
        for i in range(256):
            b = (h >> i) & 1
            sig += self.key[2 * i + b]
        return sig.hex()


class LamportPublicKey(LamportKey):
    KIND = 'LAMPORT PUBLIC KEY'

    def verify(self, m : bytes, sig : str) -> bool:
        """
        Verify a signature using this public key
        """
        bsig = bytes.fromhex(sig)
        s = [bsig[i:i+32] for i in range(0, 8192, 32)]
        h = int.from_bytes(sha256(m).digest(), byteorder='big')
        for i in range(256):
            b = (h >> i) & 1
            if sha256(s[i]).digest() != self.key[2 * i + b]:
                return False
        return True

################################################################################

key = LamportPrivateKey.keygen()
msg = b'hello world'
sig = key.sign(msg)
public_key = key.pk()
public_key.verify(msg, sig)
