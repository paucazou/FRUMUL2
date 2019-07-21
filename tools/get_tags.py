#!/usr/bin/python3
# -*-coding:Utf-8 -*
#Deus, in adjutorium meum intende
import ctypes
import os.path
import sys
import vim


class __tags:
    old_tags = []

    @classmethod
    def hi_tags(cls):
        # get content
        cb = vim.current.buffer
        content = "\n".join(cb[:])

        # get tags
        lib = ctypes.CDLL(os.path.dirname(os.path.abspath(__file__)) + "/pyvimbridge")
        lib.get_tags.restype = ctypes.c_char_p
        ret = lib.get_tags(content.encode('utf8'), cb.name.encode('utf8'))
        tags = ret.decode().split()

        # remove old tags highlighting
        if cls.old_tags:
            for elt in cls.old_tags:
                vim.command(f"syn match NONE '{elt}\S*'")
        # set new tags highlighting
        cls.old_tags = tags
        if tags:
            for elt in tags:
                vim.command(f"syn match Identifier '{elt}\S*'")

hi_tags = __tags.hi_tags
