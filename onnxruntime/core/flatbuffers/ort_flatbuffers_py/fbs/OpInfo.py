# automatically generated by the FlatBuffers compiler, do not modify

# namespace: fbs

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class OpInfo(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAsOpInfo(cls, buf, offset):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = OpInfo()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def OpInfoBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x4F\x52\x54\x4D", size_prefixed=size_prefixed)

    # OpInfo
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # OpInfo
    def OpKernelTypeStrArgs(self, j):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            x = self._tab.Vector(o)
            x += flatbuffers.number_types.UOffsetTFlags.py_type(j) * 4
            x = self._tab.Indirect(x)
            from ort_flatbuffers_py.fbs.OpIdKernelTypeStrArgsEntry import OpIdKernelTypeStrArgsEntry
            obj = OpIdKernelTypeStrArgsEntry()
            obj.Init(self._tab.Bytes, x)
            return obj
        return None

    # OpInfo
    def OpKernelTypeStrArgsLength(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.VectorLen(o)
        return 0

    # OpInfo
    def OpKernelTypeStrArgsIsNone(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        return o == 0

def OpInfoStart(builder): builder.StartObject(1)
def OpInfoAddOpKernelTypeStrArgs(builder, opKernelTypeStrArgs): builder.PrependUOffsetTRelativeSlot(0, flatbuffers.number_types.UOffsetTFlags.py_type(opKernelTypeStrArgs), 0)
def OpInfoStartOpKernelTypeStrArgsVector(builder, numElems): return builder.StartVector(4, numElems, 4)
def OpInfoEnd(builder): return builder.EndObject()
