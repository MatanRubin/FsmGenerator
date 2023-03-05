#!/usr/bin/env python
# PYTHON_ARGCOMPLETE_OK
from os import path
import yaml
import argparse
import argcomplete

DESCRIPTION = "Generate an FSM from YAML file"

parser = argparse.ArgumentParser(description=DESCRIPTION)
parser.add_argument('--input-file', '-i', help='Input YAML file')
parser.add_argument('--output-dir', '-o',
                    help='In which directory should the output be generated',
                    required=False)
parser.add_argument('--default-implementations', '-m',
                    help='generate default implementations, instead of '
                         'functions skeletons', action='store_true')

argcomplete.autocomplete(parser)
args = parser.parse_args()  # pylint: disable=C0103
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


class CodeGenerator(object):

    def __init__(self):
        self.outfile = None
        self.indent = 0

    def __str__(self):
        return "CodeGenerator"

    def set_output_file(self, outfile_path):
        if self.outfile is not None:
            self.outfile.close()
        self.outfile = open(outfile_path, 'w')

    def write_line(self, line):
        self.outfile.write(self.indent * '\t' + line + '\n')

    def comment(self, comment):
        self.write_line('// ' + comment)

    def separator(self):
        assert self.indent == 0
        self.comment(37 * '. ' + '.')

    def skip_line(self):
        self.outfile.write('\n')

    def title(self, title):
        self.separator()
        self.comment(title)
        self.skip_line()

    def open_pp_guard(self, name):
        token = name.upper()
        if not token.endswith('_H'):
            token += '_H'
        self.write_line('#ifndef {}'.format(token))
        self.write_line('#define {}'.format(token))
        self.skip_line()

    def close_pp_guard(self):
        self.skip_line()
        self.write_line('#endif')

    def indent_up(self):
        self.indent += 1

    def indent_down(self):
        self.indent -= 1

    def open_block(self):
        self.write_line('{')
        self.indent_up()

    def close_block(self, add_semicolon=False):
        self.indent_down()
        if add_semicolon:
            self.write_line('};')
        else:
            self.write_line('}')

    def gen_system_includes(self, includes_list):
        for include in includes_list:
            self.write_line('#include <' + include + '>')
        self.skip_line()

    def gen_user_includes(self, includes_list):
        for include in includes_list:
            self.write_line('#include "' + include + '"')
        self.skip_line()

    def terminate(self):
        self.outfile.close()


# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


class Transition(object):

    def __init__(self, transition_yml):
        self.input = list(transition_yml.keys())[0]
        self.next_state = list(transition_yml.values())[0]

    def __str__(self):
        return "Transition={{ input={} next_state='{}' }}"\
            .format(self.input, self.next_state)

    def __repr__(self):
        return self.__str__()
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


class State(object):

    def __init__(self, state_yml):
        self.name = state_yml["name"]
        self.transitions = []
        for trans in state_yml["transitions"]:
            self.transitions.append(Transition(trans))

    def __str__(self):
        return "State={{ name='{}' transitions={} }}".format(self.name,
                                                             self.transitions)

    def __repr__(self):
        return self.__str__()
# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


class FsmGenerator(object):

    def __init__(self, infile, outdir, default_implementations):
        self.input_file = infile
        self.output_dir = outdir
        self.default_implementations = default_implementations
        self.generator = CodeGenerator()
        self.name, self.states = self._parse_input_file()

        self.fsm_type = self.name + '_t'
        self.fsm_state_type = self.name + '_state_t'
        self.fsm_header = self.name + '.ag.h'
        self.fsm_impl = self.name + '.ag.c'
        self.fsm_states_header = self.name + '_states.ag.h'
        self.fsm_states_impl = self.name + '_states.ag.c'

        self.fsm_header_path = path.join(outdir, self.fsm_header)
        self.fsm_impl_path = path.join(outdir, self.fsm_impl)
        self.fsm_states_header_path = path.join(outdir, self.fsm_states_header)
        self.fsm_states_impl_path = path.join(outdir, self.fsm_states_impl)

    def _parse_input_file(self):
        with open(self.input_file, 'r') as stream:
            yml = yaml.safe_load(stream)
        self._validate_fsm_yml(yml)
        name = yml["name"]
        states = []
        for state in yml["states"]:
            states.append(State(state))
        return name, states

    def _validate_fsm_yml(self, yml):
        # validate yml structure is legal
        pass

    def _gen_function(self, signature, definition):
        g = self.generator
        if definition:
            g.write_line(signature)
            g.open_block()
            if self.default_implementations:
                func_name = signature.split(' ')[1].split('(')[0]
                g.write_line('printf("called function: {}\\n");'
                             .format(func_name))
            else:
                g.comment("Your code goes here")
            g.close_block()
            g.skip_line()
        else:  # declaration
            g.write_line(signature + ';')

    def _gen_next_state(self, state, definition):
        signature = "mr_fsm_state_t *{}_next_state(mr_fsm_input_type_e " \
                    "input_type, void *input)".format(state.name)
        self._gen_function(signature, definition)

    def _gen_on_entry(self, state, definition):
        signature = "void {}_on_entry()".format(state.name)
        self._gen_function(signature, definition)

    def _gen_exec(self, state, definition):
        signature = "void {}_exec(mr_fsm_input_type_e input_type, void *input)"
        signature = signature.format(state.name)
        self._gen_function(signature, definition)

    def _gen_on_exit(self, state, definition):
        signature = "void {}_on_exit()".format(state.name)
        self._gen_function(signature, definition)

    def _gen_state(self, state):
        name = state.name
        g = self.generator
        g.write_line("mr_fsm_state_t {} = ".format(name))
        g.open_block()
        g.write_line(".name = \"{}\",".format(name))
        g.write_line(".next_state = {}_next_state,".format(name))
        g.write_line(".on_entry = {}_on_entry,".format(name))
        g.write_line(".exec = {}_exec,".format(name))
        g.write_line(".on_exit = {}_on_exit,".format(name))
        g.close_block(add_semicolon=True)
        g.skip_line()

    def _gen_next_state_impl(self, state):
        g = self.generator
        g.write_line("mr_fsm_state_t *{}_next_state("
                     "mr_fsm_input_type_e input_type, "
                     "void *input)".format(state.name))
        g.open_block()
        g.write_line("if (input_type == MR_FSM_INPUT_INT)")
        g.open_block()
        g.write_line("int int_input = *(int *)input;")
        g.write_line("switch (int_input)")
        g.open_block()
        for trans in state.transitions:
            g.write_line("case {}: return &{};".format(trans.input,
                                                       trans.next_state))
        g.write_line("default: abort();")
        g.close_block()
        g.close_block()
        g.write_line("return 0;")
        g.close_block()
        g.skip_line()

    def _generate_states_impl(self):
        g = self.generator
        g.set_output_file(self.fsm_states_impl_path)

        g.gen_system_includes(['stdio.h', 'stdlib.h'])
        g.gen_user_includes(['mr_fsm_states.h'])

        for state in self.states:
            g.comment("{} functions forward declarations".format(state.name))
            self._gen_next_state(state, False)
            self._gen_on_entry(state, False)
            self._gen_exec(state, False)
            self._gen_on_exit(state, False)
            g.skip_line()

        g.title("FSM States")
        for state in self.states:
            self._gen_state(state)

        for state in self.states:
            if self.default_implementations:
                g.title("{} default implementation functions"
                        .format(state.name))
            else:
                g.title("{} Skeleton functions".format(state.name))
            self._gen_on_entry(state, True)
            self._gen_exec(state, True)
            self._gen_on_exit(state, True)

        g.title("State transition functions")
        for state in self.states:
            self._gen_next_state_impl(state)

    def _generate_states_header(self):
        g = self.generator
        g.set_output_file(self.fsm_states_header_path)

        g.open_pp_guard(self.name + "_states")
        g.gen_user_includes(["mr_fsm_states.h"])
        g.write_line('typedef mr_fsm_state_t {};'.format(self.fsm_state_type))
        for state in self.states:
            g.write_line('extern {} {};'.format(self.fsm_state_type,
                                                state.name))
        g.close_pp_guard()

    def _generate_fsm_header(self):
        g = self.generator
        g.set_output_file(self.fsm_header_path)

        fsm_type = self.fsm_type
        state_type = self.fsm_state_type

        g.open_pp_guard(self.name)
        g.gen_user_includes(['mr_fsm.h', self.fsm_states_header])
        g.write_line("typedef mr_fsm_t {};".format(fsm_type))
        g.skip_line()
        g.write_line("void {}_init({} *fsm);".format(self.name, fsm_type))
        g.write_line("void {}_destroy({} *fsm);".format(self.name, fsm_type))
        g.write_line("void {}_start({} *fsm);".format(self.name, fsm_type))
        g.write_line("void {}_start_at({} *fsm, {} *state);"
                     .format(self.name, fsm_type, state_type))
        g.write_line("void {}_stop({} *fsm);".format(self.name, fsm_type))
        g.write_line("{} *{}_get_state({} *fsm);"
                     .format(state_type, self.name, fsm_type))
        g.write_line("void {}_input({} *fsm, int input);"
                     .format(self.name, fsm_type))
        g.write_line("void {}_print(const {} *fsm);"
                     .format(self.name, fsm_type))
        g.close_pp_guard()

    def _generate_fsm_impl(self):
        g = self.generator
        g.set_output_file(self.fsm_impl_path)

        sys_includes = ['stdlib.h']
        g.gen_system_includes(sys_includes)
        g.gen_user_includes(['mr_fsm.h', self.fsm_header,
                             self.fsm_states_header])

        fsm_type = self.fsm_type
        state_type = self.fsm_state_type

        # generate init function
        g.write_line('void {}_init({} *fsm)'.format(self.name, fsm_type))
        g.open_block()
        g.write_line('unsigned int n_states = {};'.format(len(self.states)))
        g.write_line('mr_fsm_init(fsm);')
        g.write_line('fsm->name = "{}";'.format(self.name))
        g.write_line('fsm->n_states = n_states;')
        g.write_line('fsm->states = ({0} **)malloc(n_states * sizeof({0} *));'
                     .format(state_type))
        g.write_line('if (!fsm->states)')
        g.open_block()
        g.write_line('abort();')
        g.close_block()
        i = 0
        for state in self.states:
            g.write_line('fsm->states[{}] = &{};'.format(i, state.name))
            i += 1

        # TODO input file should allow defining entry state
        g.write_line('fsm->current_state = &{};'.format(self.states[0].name))
        g.close_block()
        g.skip_line()

        # generate destroy function
        g.write_line('void {}_destroy({} *fsm)'.format(self.name, fsm_type))
        g.open_block()
        g.write_line('free(fsm->states);')
        g.write_line('mr_fsm_destroy(fsm);')
        g.close_block()
        g.skip_line()

        # generate start function
        g.write_line('void {}_start({} *fsm)'.format(self.name, fsm_type))
        g.open_block()
        g.write_line('mr_fsm_start(fsm);')
        g.close_block()
        g.skip_line()

        # generate start_at function
        g.write_line("void {}_start_at({} *fsm, {} *state)"
                     .format(self.name, fsm_type, state_type))
        g.open_block()
        g.write_line('mr_fsm_start_at(fsm, state);')
        g.close_block()
        g.skip_line()

        # generate stop function
        g.write_line('void {}_stop({} *fsm)'.format(self.name, fsm_type))
        g.open_block()
        g.write_line('mr_fsm_stop(fsm);')
        g.close_block()
        g.skip_line()

        # generate get_state function
        g.write_line('{} *{}_get_state({} *fsm)'.format(state_type,
                                                        self.name, fsm_type))
        g.open_block()
        g.write_line('return mr_fsm_get_state(fsm);')
        g.close_block()
        g.skip_line()

        # generate input function
        g.write_line("void {}_input({} *fsm, int input)".format(self.name,
                                                                fsm_type))
        g.open_block()
        g.write_line('mr_fsm_input(fsm, input);')
        g.close_block()
        g.skip_line()

        # generate print function
        g.write_line('void {}_print(const {} *fsm)'.format(self.name,
                                                           fsm_type))
        g.open_block()
        g.write_line('mr_fsm_print(fsm);')
        g.close_block()
        g.skip_line()

    def generate_fsm(self):
        self._generate_fsm_header()
        self._generate_fsm_impl()

    def generate_states(self):
        self._generate_states_header()
        self._generate_states_impl()

    def terminate(self):
        self.generator.terminate()


# . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
def main():
    fsm_gen = FsmGenerator(args.input_file, args.output_dir,
                           args.default_implementations)
    fsm_gen.generate_fsm()
    fsm_gen.generate_states()
    fsm_gen.terminate()

if __name__ == '__main__':
    main()
