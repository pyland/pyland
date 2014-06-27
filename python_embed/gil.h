class GIL {
    public:
        GIL() { gstate = PyGILState_Ensure(); }
        ~GIL() { PyGILState_Release(gstate); }

    private:
        GIL(const GIL&);
        PyGILState_STATE gstate;
};
